#ifndef MODEL_H
#define MODEL_H

#include "components.h"

typedef struct {
    long entity_id;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int IBO;
} OpenGLBuffers;

void init_model_buffers(OpenGLBuffers* buffers, const Mesh* mesh);
void delete_model_buffers(OpenGLBuffers* model);
Mat4 transform_matrix(const Transform* transform);
void draw_model(const OpenGLBuffers* buffers, const Mesh* mesh);

#endif /* ! MODEL_H */
