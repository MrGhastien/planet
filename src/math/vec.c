#include "vec.h"
#include <math.h>

Vec3 vec3_add(Vec3 a, Vec3 b) {
    return (Vec3) {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
    };
}

Vec3 vec3_diff(Vec3 a, Vec3 b) {
    return (Vec3) {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
    };
}

void vec3_mult(Vec3* v, float f) {
    v->x *= f;
    v->y *= f;
    v->z *= f;
}

Vec3 vec3_mult1(Vec3 v, float f) {
    vec3_mult(&v, f);
    return v;
}

Vec3 vec3_neg(Vec3 v) {
    return (Vec3) {
        .x = -v.x,
        .y = -v.y,
        .z = -v.z,
    };
}

float vec3_length(Vec3 v) {
    return sqrtf(vec3_sqrlength(v));
}

float vec3_sqrlength(Vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

Vec3 vec3_normalize(Vec3 v) {
    if(v.x == 0 && v.y == 0 && v.z == 0)
        return v;
    float norm = vec3_length(v);
    return (Vec3) {
        .x = v.x / norm,
        .y = v.y / norm,
        .z = v.z / norm
    };
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return (Vec3) {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x,
    };
}

float vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 vec3_lerp(Vec3 a, Vec3 b, float t) {
    vec3_mult(&a, 1 - t);
    vec3_mult(&b, t);

    return vec3_add(a, b);
}
