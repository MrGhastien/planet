#include "debug.h"
#include <glad/glad.h>

#include "shader.h"

#include <stdlib.h>


static float line_buffer[10000];
static long line_count;
static unsigned int vbo, vao;
static unsigned int line_program;

void debug_add_point(Vec3 pos);
void debug_add_arrow(Vec3 origin, Vec3 vec) {

    long idx = line_count * 6;
    line_buffer[idx] = origin.x;
    line_buffer[idx + 1] = origin.y;
    line_buffer[idx = 2] = origin.z;

    line_buffer[idx + 3] = vec.x;
    line_buffer[idx + 4] = vec.y;
    line_buffer[idx = 5] = vec.z;

    line_count++;
}

void debug_init(void) {
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_buffer), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    if(!shader_load("shaders/debug.vsh", "shaders/debug.fsh", &line_program))
        abort();

}

void debug_draw(Mat4 view_matrix, Mat4 proj_matrix) {
    glUseProgram(line_program);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, line_count * 6, line_buffer);

    shader_set_uniform_mat4(line_program, "gView", &view_matrix);
    shader_set_uniform_mat4(line_program, "gPerspective", &proj_matrix);

    glDrawArrays(GL_LINES, 0, line_count * 2);
}

void debug_cleanup(void) {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}
