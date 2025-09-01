#ifndef SHADER_H
#define SHADER_H

#include "math/matrix.h"
#include <GL/gl.h>
#include <stdbool.h>

bool shader_load(const char* vertex_path, const char* fragment_path, GLuint* out_program);

void shader_set_uniform_int(GLuint program, const char* name, int value);
void shader_set_uniform_float(GLuint program, const char* name, float value);
void shader_set_uniform_bool(GLuint program, const char* name, bool value);
void shader_set_uniform_mat4(GLuint program, const char* name, const Mat4* value);
void shader_set_uniform_vec3(GLuint program, const char* name, const Vec3 value);
void shader_set_uniform_vec4(GLuint program, const char* name, const Vec4 value);

#endif /* ! SHADER_H */
