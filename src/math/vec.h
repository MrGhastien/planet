#ifndef VEC_H
#define VEC_H


typedef struct {
    float x, y;
} Vec2;

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    double x, y, z;
} DVec3;

typedef struct {
    float x, y, z, w;
} Vec4;

Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_diff(Vec3 a, Vec3 b);
void vec3_mult(Vec3* v, float f);
Vec3 vec3_mult1(Vec3 v, float f);

Vec3 vec3_neg(Vec3 v);

Vec3 vec3_cross(Vec3 a, Vec3 b);
float vec3_dot(Vec3 a, Vec3 b);

Vec3 vec3_normalize(Vec3 v);
float vec3_length(Vec3 v);
float vec3_sqrlength(Vec3 v);

Vec3 vec3_lerp(Vec3 a, Vec3 b, float t);

#define VEC3_ZERO ((Vec3) {.x = 0, .y = 0, .z = 0})

#endif /* ! VEC_H */
