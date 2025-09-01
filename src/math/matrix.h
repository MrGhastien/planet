#ifndef MATRIX_H
#define MATRIX_H

#include "vec.h"

typedef struct {
    float data[4 * 4];
} Mat4;

typedef struct {
    float data[3 * 3];
} Mat3;

Vec3 mat4_vec3_mult(Mat4 m, Vec3 v);
Vec4 mat4_vec4_mult(Mat4 m, Vec4 v);
Mat4 mat4_mult(Mat4 a, Mat4 b);

Mat4 mat4_scale(Vec3 v);
Mat4 mat4_trans(Vec3 v);
Mat4 mat4_perpective(float fov, float aspect_ratio, float near, float far);

Mat3 mat3_inverse(Mat3 m);
Vec3 mat3_vec_mult(Mat3 m, Vec3 v);

#define MAT4_ZERO (Mat4){0}

#define MAT4_ID                                                                                    \
    (Mat4) {                                                                                       \
        .data = {                                                                                  \
            [0] = 1,                                                                               \
            [1] = 0,                                                                               \
            [2] = 0,                                                                               \
            [3] = 0,                                                                               \
            [4] = 0,                                                                               \
            [5] = 1,                                                                               \
            [6] = 0,                                                                               \
            [7] = 0,                                                                               \
            [8] = 0,                                                                               \
            [9] = 0,                                                                               \
            [10] = 1,                                                                              \
            [11] = 0,                                                                              \
            [12] = 0,                                                                              \
            [13] = 0,                                                                              \
            [14] = 0,                                                                              \
            [15] = 1,                                                                              \
        }                                                                                          \
    }

#endif /* ! MATRIX_H */
