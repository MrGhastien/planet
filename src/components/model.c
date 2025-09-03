#include "model.h"
#include <glad/glad.h>
#include <stdlib.h>

void init_model_buffers(OpenGLBuffers* buffers, const Mesh* mesh) {

    glGenBuffers(1, &buffers->VBO);
    glGenBuffers(1, &buffers->IBO);
    glGenVertexArrays(1, &buffers->VAO);

    glBindVertexArray(buffers->VAO);

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

    glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO);
    glBufferData(GL_ARRAY_BUFFER,  total_size, flatten_vertices, GL_STATIC_DRAW);
    free(flatten_vertices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof *mesh->indices * mesh->index_count, mesh->indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void delete_model_buffers(OpenGLBuffers* buffers) {
    glDeleteVertexArrays(1, &buffers->VAO);
    glDeleteBuffers(1, &buffers->VBO);
    glDeleteBuffers(1, &buffers->IBO);
}

Mat4 transform_matrix(const Transform* transform) {
    Mat4 trans_matrix = mat4_trans(transform->position);
    Mat4 rot_matrix = quaternion_to_matrix(transform->rotation);
    Mat4 scale_matrix = mat4_scale(transform->scale);
    return mat4_mult(trans_matrix, mat4_mult(rot_matrix, scale_matrix));
}

void draw_model(const OpenGLBuffers* buffers, const Mesh* mesh) {
    glBindVertexArray(buffers->VAO);
    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
}
