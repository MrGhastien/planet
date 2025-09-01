#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#define PI_F 3.14159265359f

static inline float deg_to_rad(float degs) {
    return (degs * PI_F) / 180.0f;
}

static inline float sqr(float x) {
    return x * x;
}

static inline double dsqr(double x) {
    return x * x;
}

static inline float lerp(float a, float b, float t) {
    return a * (1 - t) + b * t;
}

#endif /* ! MATH_UTILS_H */
