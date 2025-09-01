#ifndef MODEL_H
#define MODEL_H

#include "math/quaternion.h"
#include "math/vec.h"
#include "mesh.h"

typedef struct {
    Vec3 position;
    Vec3 scale;
    Quaternion rotation;
} Transform;

typedef struct {
    Transform transform;
    Mesh mesh;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int IBO;
} Model;

void init_model_buffers(Model* model);
void delete_model_buffers(Model* model);
Mat4 transform_matrix(const Transform* transform);
void draw_model(const Model* model);

#endif /* ! MODEL_H */
