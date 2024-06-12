#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <cmath>
#include <vector>
#include <array>
#include <iostream>
#include "common.h"

class Renderer : public QWidget {
    Q_OBJECT

public:
    Renderer(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(2 * SCREEN_WIDTH, 2 * SCREEN_HEIGHT);
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &Renderer::updateFrame);
        m_timer->start(16);

        m_pos   = vector2(2.0f, 2.0f);
        m_dir   = vector2(-1.0f, 0.1f);
        m_plane = vector2(0.0f, 0.66f);

        m_image = QImage(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_RGB32);
        setWindowTitle(QString("Raycasting"));;
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(&m_image);
        render(painter);
        QPainter painter2(this);
        painter2.drawImage(0, 0, m_image.scaled(2 * SCREEN_WIDTH, 2 * SCREEN_HEIGHT));
    }

    void keyPressEvent(QKeyEvent *event) override {
        const float rotspeed = 3.0f * 0.016f;
        const float movespeed = 3.0f * 0.016f;

        if (event->key() == Qt::Key_Left) {
            rotate(rotspeed);
        } else if (event->key() == Qt::Key_Right) {
            rotate(-rotspeed);
        } else if (event->key() == Qt::Key_Up) {
            m_pos.x += m_dir.x * movespeed;
            m_pos.y += m_dir.y * movespeed;
        } else if (event->key() == Qt::Key_Down) {
            m_pos.x -= m_dir.x * movespeed;
            m_pos.y -= m_dir.y * movespeed;
        }
        update();
        QWidget::keyPressEvent(event);
    }

private slots:
    void updateFrame() {
       update();
    }

private:
    void verline(QPainter &painter, int x, int y0, int y1, QColor color) {
        painter.setPen(color);
        for (int y = y0; y <= y1; ++y) {
            painter.drawPoint(x, y);
        }
    }

    void render(QPainter &painter) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            float xcam = (2.0f * (x / (float) (SCREEN_WIDTH))) - 1.0f;
            vector2 rayDir = vector2(
                m_dir.x + m_plane.x * xcam,
                m_dir.y + m_plane.y * xcam);

            vector2  mapPos   = m_pos;
            vector2i mapCoord = vector2i(int(mapPos.x), int(mapPos.y));

            vector2 deltaDist = vector2(
                std::fabs(1.0f / rayDir.x),
                std::fabs(1.0f / rayDir.y));
            vector2 sideDist  = vector2(
                rayDir.x < 0 ? (mapPos.x - mapCoord.x) * deltaDist.x : (mapCoord.x + 1.0 - mapPos.x) * deltaDist.x,
                rayDir.y < 0 ? (mapPos.y - mapCoord.y) * deltaDist.y : (mapCoord.y + 1.0 - mapPos.y) * deltaDist.y
            );

            vector2i step = vector2i(sign(rayDir.x), sign(rayDir.y));
            hitStruct hit;

            while (!hit.val) {
                if (sideDist.x < sideDist.y) {
                    sideDist.x += deltaDist.x;
                    mapCoord.x += step.x;
                    hit.side = 0;
                } else {
                    sideDist.y += deltaDist.y;
                    mapCoord.y += step.y;
                    hit.side = 1;
                }

                if (mapCoord.x < 0 || mapCoord.x >= MAP_SIZE || mapCoord.y < 0 || mapCoord.y >= MAP_SIZE) {
                    qCritical() << "DDA out of bounds";
                    std::exit(1);
                }


                hit.val = MAPDATA[mapCoord.y * MAP_SIZE + mapCoord.x];
            }

            QColor color;
            switch (hit.val) {
                case 1: color = QColor(255, 0, 0); break;
                case 2: color = QColor(0, 255, 0); break;
                case 3: color = QColor(255, 0, 0); break;
                case 4: color = QColor(255, 0, 255); break;
            }
            if (hit.side == 1) {
                color.setRed(color.red() * 0.75);
                color.setGreen(color.green() * 0.75);
                color.setBlue(color.blue() * 0.75);
            }

            hit.pos = vector2(mapPos.x + sideDist.x, mapPos.y + sideDist.y);

            float perpWallDist = (hit.side == 0) ? (sideDist.x - deltaDist.x) : (sideDist.y - deltaDist.y);
            int lineHeight = static_cast<int>(SCREEN_HEIGHT / perpWallDist);
            int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
            int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;

            drawStart = max(drawStart, 0);
            drawEnd = min(drawEnd, SCREEN_HEIGHT - 1);

            verline(painter, x, 0, drawStart, QColor(32, 32, 32));
            verline(painter, x, drawStart, drawEnd, color);
            verline(painter, x, drawEnd, SCREEN_HEIGHT, QColor(80, 80, 80));
        }
    }

    void rotate(float rot) {
        float oldDirX = m_dir.x;
        m_dir.x = m_dir.x * cos(rot) - m_dir.y * sin(rot);
        m_dir.y = oldDirX * sin(rot) + m_dir.y * cos(rot);

        float oldPlaneX = m_plane.x;
        m_plane.x = m_plane.x * cos(rot) - m_plane.y * sin(rot);
        m_plane.y = oldPlaneX * sin(rot) + m_plane.y * cos(rot);
    }
private:
    QTimer *m_timer;
    vector2 m_pos, m_dir, m_plane;
    QImage m_image;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Renderer renderer;
    renderer.show();

    return app.exec();
}
#include "main.moc"
