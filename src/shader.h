#ifndef SHADER_H
#define SHADER_H

#include <GL/gl.h>
#include <stdbool.h>

bool shader_load(const char* vertex_path, const char* fragment_path, GLuint* out_program);

void shader_set_uniform_int(GLuint program, const char* name, int value);
void shader_set_uniform_float(GLuint program, const char* name, float value);
void shader_set_uniform_bool(GLuint program, const char* name, bool value);

#endif /* ! SHADER_H */
