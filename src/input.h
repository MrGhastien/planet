#ifndef INPUT_H
#define INPUT_H

#include "camera.h"
#include <GLFW/glfw3.h>

void init_input(GLFWwindow* window);
void processInput(GLFWwindow* window, Camera* cam);

#endif /* ! INPUT_H */
