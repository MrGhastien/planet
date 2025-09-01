#include "model.h"
#include <glad/glad.h>
#include <stdlib.h>

void init_model_buffers(Model* model) {

    glGenBuffers(1, &model->VBO);
    glGenBuffers(1, &model->IBO);
    glGenVertexArrays(1, &model->VAO);

    glBindVertexArray(model->VAO);

    Mesh* mesh = &model->mesh;
    float* flatten_vertices;
    size_t total_size = sizeof *flatten_vertices * (mesh->vertex_count * 3 + mesh->normal_count * 3);
    flatten_vertices = malloc(total_size);

    for(unsigned int i = 0; i < mesh->vertex_count; i++) {
        Vec3 v = mesh->vertices[i];
        Vec3 n = mesh->normals[i];
        flatten_vertices[i * 6] = v.x;
        flatten_vertices[i * 6 + 1] = v.y;
        flatten_vertices[i * 6 + 2] = v.z;

        flatten_vertices[i * 6 + 3] = n.x;
        flatten_vertices[i * 6 + 4] = n.y;
        flatten_vertices[i * 6 + 5] = n.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
    glBufferData(GL_ARRAY_BUFFER,  total_size, flatten_vertices, GL_STATIC_DRAW);
    free(flatten_vertices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof *mesh->indices * mesh->index_count, model->mesh.indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void delete_model_buffers(Model* model) {
    glDeleteVertexArrays(1, &model->VAO);
    glDeleteBuffers(1, &model->VBO);
    glDeleteBuffers(1, &model->IBO);
}

Mat4 transform_matrix(const Transform* transform) {
    Mat4 trans_matrix = mat4_trans(transform->position);
    Mat4 rot_matrix = quaternion_to_matrix(transform->rotation);
    Mat4 scale_matrix = mat4_scale(transform->scale);
    return mat4_mult(trans_matrix, mat4_mult(rot_matrix, scale_matrix));
}

void draw_model(const Model* model) {
    glBindVertexArray(model->VAO);
    glDrawElements(GL_TRIANGLES, model->mesh.index_count, GL_UNSIGNED_INT, 0);
}
