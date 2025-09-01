#ifndef QUATERNION_H
#define QUATERNION_H

#include "matrix.h"

typedef struct {
    float r;
    float i;
    float j;
    float k;
} Quaternion;

Quaternion quaternion_add(Quaternion a, Quaternion b);
Quaternion quaternion_sub(Quaternion a, Quaternion b);
Quaternion quaternion_mult(Quaternion a, Quaternion b);
Quaternion quaternion_multf(Quaternion q, float f);

Quaternion quaternion_conj(Quaternion q);
Quaternion quaternion_inv(Quaternion q);

Quaternion quaternion_unit(float angle, Vec3 axis);
Vec3 quaternion_rotate(Quaternion q, Vec3 v);

Mat4 quaternion_to_matrix(Quaternion q);

Quaternion quaternion_lerp(Quaternion a, Quaternion b, float t);

float quaternion_length(Quaternion q);
Quaternion quaternion_normalize(Quaternion q);

#endif /* ! QUATERNION_H */
