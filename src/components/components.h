#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "math/quaternion.h"
#include "math/vec.h"

#include <stdbool.h>
#include <glad/glad.h>

enum ComponentType {
    COMPONENT_TRANSFORM,
    COMPONENT_PHYSICS,
    COMPONENT_SHIP,
    COMPONENT_MESH,
    COMPONENT_OPENGL,
    _COMPONENT_COUNT,
};    

typedef struct {
    long components[_COMPONENT_COUNT];
} Thing;

typedef struct {
    long entity_id;
    unsigned int vertex_count;
    unsigned int normal_count;
    unsigned int index_count;
    Vec3* vertices;
    Vec3* normals;
    unsigned int* indices;
} Mesh;

typedef struct {
    long entity_id;
    Vec3 position;
    Vec3 scale;
    Quaternion rotation;
} Transform;

void components_init(void);
void components_cleanup(void);

void* components_get(enum ComponentType type, long entity_id);
void components_attach(long entity_id, enum ComponentType type, const void* data);
void components_update(double delta_time);
void components_render(double delta_time, GLuint program);

long create_thing(void);

#endif /* ! COMPONENTS_H */
