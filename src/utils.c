#include "utils.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

void report_GLFW_error(const char* func_name) {
    const char* desc;
    int code = glfwGetError(&desc);
    fprintf(stderr, "%s() failed with error code %i", func_name, code);
    if (desc)
        fprintf(stderr, ": %s\n", desc);
    else
        fputc('\n', stderr);
}
/*

void draw_line(Vec3 from, Vec3 to, Vec4 color) {
    glDrawArrays(GL_LINES, 0, );
}
*/
