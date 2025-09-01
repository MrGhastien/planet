#ifndef MESH_H
#define MESH_H

#include "math/vec.h"

typedef struct {
    unsigned int vertex_count;
    unsigned int normal_count;
    unsigned int index_count;
    Vec3* vertices;
    Vec3* normals;
    unsigned int* indices;
} Mesh;

Mesh generate_cubesphere(float radius, int resolution, Vec3 up);
Mesh generate_quad(float size, Vec3 up);

#endif /* ! MESH_H */
