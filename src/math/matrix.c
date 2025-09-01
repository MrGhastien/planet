#include "matrix.h"

#include "utils.h"

#include <math.h>

Vec3 mat4_vec3_mult(Mat4 m, Vec3 v) {
    Vec4 vec4 = {
        .x = v.x,
        .y = v.y,
        .z = v.z,
        .w = 1.0f,
    };
    vec4 = mat4_vec4_mult(m, vec4);
    return (Vec3) {
        .x = vec4.x,
        .y = vec4.y,
        .z = vec4.z,
    };
}

Vec4 mat4_vec4_mult(Mat4 m, Vec4 v) {
    return (Vec4) {
        .x = m.data[0] * v.x + m.data[1] * v.y + m.data[2] * v.z + m.data[3] * v.w,
        .y = m.data[4] * v.x + m.data[5] * v.y + m.data[6] * v.z + m.data[7] * v.w,
        .z = m.data[8] * v.x + m.data[9] * v.y + m.data[10] * v.z + m.data[11] * v.w,
        .w = m.data[12] * v.x + m.data[13] * v.y + m.data[14] * v.z + m.data[15] * v.w,
    };
}

Mat4 mat4_mult(Mat4 a, Mat4 b) {
    Mat4 res = {0};

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                res.data[(i << 2) + j] += a.data[(i << 2) + k] * b.data[(k << 2) + j];
            }
        }
    }
    return res;
}

Mat4 mat4_scale(Vec3 v) {
    return (Mat4) {
        .data = {
            [0] = v.x,
            [1] = 0,
            [2] = 0,
            [3] = 0,
            [4] = 0,
            [5] = v.y,
            [6] = 0,
            [7] = 0,
            [8] = 0,
            [9] = 0,
            [10] = v.z,
            [11] = 0,
            [12] = 0,
            [13] = 0,
            [14] = 0,
            [15] = 1,
        }
    };
}

/*
  1 0 0 x
  0 1 0 y
  0 0 1 z
  0 0 0 1
 */

Mat4 mat4_trans(Vec3 v) {
    return (Mat4) {
        .data = {
            [0] = 1,
            [1] = 0,
            [2] = 0,
            [3] = v.x,
            [4] = 0,
            [5] = 1,
            [6] = 0,
            [7] = v.y,
            [8] = 0,
            [9] = 0,
            [10] = 1,
            [11] = v.z,
            [12] = 0,
            [13] = 0,
            [14] = 0,
            [15] = 1,
        }
    };
}

Mat4 mat4_perpective(float fov, float aspect_ratio, float near, float far) {
    (void)aspect_ratio;
    (void)near;
    (void)far;
    float tangent = tanf(deg_to_rad(fov / 2.0f));
    float cotangent = 1.0f / tangent;
    return (Mat4) {
        .data = {
            [0] = cotangent / aspect_ratio ,
            [5] = cotangent,
            [10] = -(far + near) / (far - near),
            [11] = -2 * far * near / (far - near),
            [14] = -1,
            [15] = 0,
        }
    };
}

static float mat3_det(Mat3 m) {
    float* e = m.data;
    return e[2] * e[4] * e[6]
        + e[1] * e[5] * e[6]
        + e[2] * e[3] * e[7]
        - e[0] * e[5] * e[7]
        - e[1] * e[3] * e[8]
        + e[0] * e[4] * e[8];
}

Mat3 mat3_inverse(Mat3 m) {
    float neg_det = -mat3_det(m);
    float* e = m.data;

    return (Mat3) {
        .data = {
            [0] = (e[5] * e[7] - e[4] * e[8]) / neg_det,
            [1] = (-e[2] * e[7] + e[1] * e[8]) / neg_det,
            [2] = (e[2] * e[4] - e[1] * e[5]) / neg_det,
            [3] = (-e[5] * e[6] + e[3] * e[8]) / neg_det,
            [4] = (e[2] * e[6] - e[0] * e[8]) / neg_det,
            [5] = (-e[2] * e[3] - e[0] * e[5]) / neg_det,
            [6] = (e[4] * e[6] - e[3] * e[7]) / neg_det,
            [7] = (-e[1] * e[6] - e[0] * e[7]) / neg_det,
            [8] = (e[1] * e[3] - e[0] * e[4]) / neg_det,
        }
    };
}

Vec3 mat3_vec_mult(Mat3 m, Vec3 v) {
    return (Vec3) {
        .x = m.data[0] * v.x + m.data[1] * v.y + m.data[2] * v.z,
        .y = m.data[3] * v.x + m.data[4] * v.y + m.data[5] * v.z,
        .z = m.data[6] * v.x + m.data[7] * v.y + m.data[8] * v.z,
    };
}
