#ifndef COMMON_H
#define COMMON_H
#include <array>
#include <cmath>

constexpr static const int SCREEN_WIDTH   = 384;
constexpr static const int SCREEN_HEIGHT  = 216;

const unsigned int MAP_SIZE = 8;

struct vector2 {
    vector2(float _x = 0.0f, float _y = 0.0f)
        : x(_x), y(_y) {}
    float x, y;
};

struct vector2i {
    vector2i(int _x = 0, int _y = 0)
        : x(_x), y(_y) {}
    int x, y;
};

struct hitStruct
{
    hitStruct(int _val, int _side, vector2 _pos)
        : val(_val), side(_side), pos(_pos){}
    hitStruct()
        : val(0), side(0), pos(0.0f, 0.0f){}
    int val, side;
    vector2 pos;
};

static std::array<unsigned int, MAP_SIZE * MAP_SIZE> MAPDATA = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 3, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 2, 0, 4, 4, 0, 1,
    1, 0, 0, 0, 4, 0, 0, 1,
    1, 0, 3, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};


float dot(vector2 v0, vector2 v1) {
    return ( (v0.x * v1.x) + (v0.y * v1.y) );
}

int dot(vector2i v0, vector2i v1) {
    return ( (v0.x * v1.x) + (v0.y * v1.y) );
}

float length(vector2 v) {
    return std::sqrtf(dot(v,v));
}

int length(vector2i v) {
    return std::sqrtf(dot(v,v));
}

template<typename T>
T normalize(T v) {
    return T(v.x / length(v), v.y / length(v));
}


template<typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

template<typename T>
T min(T a, T b) {
    return a < b ? a : b;
}

int sign(float a) {
    if (a < 0.0f) {
        return -1;
    }
    else if (a > 0.0) {
        return 1;
    }
    else
    {
        return 0;
    }
}




#endif // COMMON_H
