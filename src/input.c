#include "math/quaternion.h"
#include "math/vec.h"
#include "camera.h"

#include <GLFW/glfw3.h>
#include <stdbool.h>

static Vec3 cam_pos_save;
static Quaternion cam_rot_save;

static bool key_presses[65536];
static bool key_ack[65536];

extern float interp_factor;

static bool get_key(int key) {
    bool status = key_presses[key];
    return status;
}

static bool get_key_single(int key) {
    bool status = key_presses[key] && !key_ack[key];
    key_ack[key] = true;
    return status;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window;
    (void)scancode;
    (void)mods;
    key_presses[key] = action == GLFW_PRESS;
    key_ack[key] = false;
}

void init_input(GLFWwindow* window) {
    glfwSetKeyCallback(window, &key_callback);
}

void processInput(GLFWwindow* window, Camera* cam) {
    if (get_key(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);

    Vec3 cam_delta = {0};
    float vertical_delta = 0;

    if (get_key_single(GLFW_KEY_P)) {
        piloting = !piloting;
        if (piloting) {
            cam_pos_save = cam->position;
            cam_rot_save = cam->rotation;
        } else {
            cam->position = cam_pos_save;
            cam->rotation = cam_rot_save;
        }
    }

    if (get_key(GLFW_KEY_W))
        cam_delta = (Vec3) {.x = 0, .y = 0, .z = -1};
    if (get_key(GLFW_KEY_D))
        cam_delta = vec3_add(cam_delta, (Vec3) {.x = 1, .y = 0, .z = 0});
    if (get_key(GLFW_KEY_A))
        cam_delta = vec3_add(cam_delta, (Vec3) {.x = -1, .y = 0, .z = 0});
    if (get_key(GLFW_KEY_S))
        cam_delta = vec3_add(cam_delta, (Vec3) {.x = 0, .y = 0, .z = 1});
    if (get_key(GLFW_KEY_SPACE))
        vertical_delta += 1;
    if (get_key(GLFW_KEY_LEFT_SHIFT))
        vertical_delta += -1;

    if(get_key(GLFW_KEY_UP))
        interp_factor *= 1.1;

    if(get_key(GLFW_KEY_DOWN))
        interp_factor *= 0.9;

    if(interp_factor < 0)
        interp_factor = 0;
    else if(interp_factor > 1)
        interp_factor = 1;

    cam_delta = vec3_normalize(cam_delta);
    cam_delta = quaternion_rotate(cam->rotation, cam_delta);
    vec3_mult(&cam_delta, 0.15f);
    vertical_delta *= 0.15f;
    cam->position = vec3_add(cam->position, cam_delta);
    cam->position = vec3_add(cam->position, (Vec3) {.y = vertical_delta});
}
