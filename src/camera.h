#ifndef CAMERA_H
#define CAMERA_H

#include "math/vec.h"
#include "math/quaternion.h"

#include <stdbool.h>

extern bool piloting;

typedef struct {
    Quaternion actual_rotation;
    Vec3 actual_position;
    float actual_fov;

    Quaternion rotation;
    Vec3 position;
    float fov;
} Camera;

#endif /* ! CAMERA_H */
