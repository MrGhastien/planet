#include "quaternion.h"

#include "utils.h"

#include <math.h>

Quaternion quaternion_add(Quaternion a, Quaternion b) {
    return (Quaternion) {
        .r = a.r + b.r,
        .i = a.i + b.i,
        .j = a.j + b.j,
        .k = a.k + b.k,
    };
}
Quaternion quaternion_sub(Quaternion a, Quaternion b) {
    return (Quaternion) {
        .r = a.r - b.r,
        .i = a.i - b.i,
        .j = a.j - b.j,
        .k = a.k - b.k,
    };
}
Quaternion quaternion_mult(Quaternion a, Quaternion b) {
    return (Quaternion) {
        .r = a.r * b.r - a.i * b.i - a.j * b.j - a.k * b.k,
        .i = a.r * b.i + a.i * b.r + a.k * b.j - a.j * b.k,
        .j = a.r * b.j + a.j * b.r + a.i * b.k - a.k * b.i,
        .k = a.r * b.k + a.k * b.r + a.j * b.i - a.i * b.j,
    };
}

Quaternion quaternion_multf(Quaternion q, float f) {
    return (Quaternion) {
        .r = q.r * f,
        .i =  q.i * f,
        .j = q.j * f,
        .k = q.k * f,
    };
}

Quaternion quaternion_conj(Quaternion q) {
    return (Quaternion) {
        .r = q.r,
        .i = -q.i,
        .j = -q.j,
        .k = -q.k,
    };
}

static inline float sqr_norm(Quaternion q) {
    return q.r * q.r + q.i * q.i + q.j * q.j + q.k * q.k;
}

Quaternion quaternion_inv(Quaternion q) {
    return quaternion_multf(quaternion_conj(q), 1.0f / sqr_norm(q));
}

Quaternion quaternion_unit(float angle, Vec3 axis) {
    //float rad_angle = deg_to_rad(angle / 2);
    float sin_angle = sinf(angle);
    return (Quaternion) {
        .r = cosf(angle),
        .i = axis.x * sin_angle,
        .j = axis.y * sin_angle,
        .k = axis.z * sin_angle,
    };
}

Vec3 quaternion_rotate(Quaternion q, Vec3 v) {
    Quaternion q_inv = quaternion_inv(q);
    Quaternion q_vec = {.r = 0, .i = v.x, .j = v.y, .k = v.z};

    Quaternion res = quaternion_mult(q, quaternion_mult(q_vec, q_inv));

    return (Vec3) {
        .x = res.i,
        .y = res.j,
        .z = res.k,
    };
}

Mat4 quaternion_to_matrix(Quaternion q) {
    return (Mat4) {
        .data = {
            [0] = 1 - 2 * (q.j * q.j + q.k * q.k),
            [1] = 2 * q.i * q.j - 2 * q.r * q.k,
            [2] = 2 * (q.i * q.k + q.r * q.j),
            [3] = 0,
            [4] = 2 * (q.i * q.j + q.r * q.k),
            [5] = 1 - 2 * (q.i * q.i + q.k * q.k),
            [6] = 2 * q.j * q.k - 2 * q.r * q.i,
            [7] = 0,
            [8] = 2 * q.i * q.k - 2 * q.r * q.j,
            [9] = 2 * (q.j * q.k + q.r * q.i),
            [10] = 1 - 2 * (q.i * q.i + q.j * q.j),
            [11] = 0,
            [12] = 0,
            [13] = 0,
            [14] = 0,
            [15] = 1,
        },
    };
}

Quaternion quaternion_lerp(Quaternion a, Quaternion b, float t) {
    Quaternion ta =  quaternion_multf(a, 1 - t);
    Quaternion tb =  quaternion_multf(b, t);

    return quaternion_add(ta, tb);
}

float quaternion_length(Quaternion q) {
    return sqrtf(q.r * q.r + q.i * q.i + q.j * q.j + q.k * q.k);
}
Quaternion quaternion_normalize(Quaternion q) {
    return quaternion_multf(q, 1 / quaternion_length(q));
}
