#include <glad/glad.h>

#include "shader.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

static char* load_file(const char* path, size_t* out_length) {
    FILE* file = fopen(path, "r");

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* contents = malloc((size + 1) * sizeof(char));

    size_t res = fread(contents, sizeof(char), size, file);
    if(res < size) {
        fprintf(stderr, "Failed to read file '%s': %s\n", path, strerror(errno));
        return NULL;
    }

    fclose(file);

    *out_length = size;

    return contents;
}

static bool compile_shader(const char* contents, size_t length, GLuint shader_handle) {
    GLint int_length = length;
    glShaderSource(shader_handle, 1, &contents, &int_length);
    glCompileShader(shader_handle);

    GLint result;

    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &result);


    if(!result) {
        int info_log_length;
        glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &info_log_length);
        char* info_log = malloc(info_log_length + 1);
        glGetShaderInfoLog(shader_handle, info_log_length, NULL, info_log);
        fprintf(stderr, "Shader compilation failed: %s\n", info_log);
        return false;
    }
    return true;
}

static bool create_program(GLuint vsh_handle, GLuint fsh_handle, GLuint* out_program_handle) {
    GLuint program_handle = glCreateProgram();

    glAttachShader(program_handle, vsh_handle);
    glAttachShader(program_handle, fsh_handle);
    glLinkProgram(program_handle);

    GLint result;
    glGetProgramiv(program_handle, GL_LINK_STATUS, &result);
    if(!result) {
        int info_log_length;
        glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &info_log_length);
        char* info_log = malloc(info_log_length + 1);
        glGetProgramInfoLog(program_handle, info_log_length, NULL, info_log);
        fprintf(stderr, "Program creation failed: %s\n", info_log);
        return false;
    }

    *out_program_handle = program_handle;

    return true;
}

bool shader_load(const char* vertex_path, const char* fragment_path, GLuint* out_program) {
    GLuint vsh_handle = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsh_handle = glCreateShader(GL_FRAGMENT_SHADER);

    size_t vsh_size;
    size_t fsh_size;
    char* vsh_contents = load_file(vertex_path, &vsh_size);
    if(!vsh_contents)
        return false;

    char* fsh_contents = load_file(fragment_path, &fsh_size);
    if(!fsh_contents)
        return false;

    if(!compile_shader(vsh_contents, vsh_size, vsh_handle))
        return false;
    if(!compile_shader(fsh_contents, fsh_size, fsh_handle))
        return false;

    GLuint program_handle;
    if(!create_program(vsh_handle, fsh_handle, &program_handle))
        return false;

    free(vsh_contents);
    free(fsh_contents);

    glDeleteShader(vsh_handle);
    glDeleteShader(fsh_handle);

    *out_program = program_handle;

    return true;
}

void shader_set_uniform_int(GLuint program, const char* name, int value) {
    glUniform1i(glGetUniformLocation(program, name), value);
}
void shader_set_uniform_float(GLuint program, const char* name, float value) {
    glUniform1f(glGetUniformLocation(program, name), value);
}
void shader_set_uniform_bool(GLuint program, const char* name, bool value) {
    glUniform1i(glGetUniformLocation(program, name), value ? 1 : 0);
}
void shader_set_uniform_mat4(GLuint program, const char* name, const Mat4* value) {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_TRUE, value->data);
}
void shader_set_uniform_vec3(GLuint program, const char* name, const Vec3 value) {
    glUniform3f(glGetUniformLocation(program, name), value.x, value.y, value.z);
}
void shader_set_uniform_vec4(GLuint program, const char* name, const Vec4 value) {
    glUniform4f(glGetUniformLocation(program, name), value.x, value.y, value.z, value.w);
}
