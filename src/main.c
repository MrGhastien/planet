#include <glad/glad.h>

#include "camera.h"
#include "debug.h"
#include "input.h"
#include "math/matrix.h"
#include "math/quaternion.h"
#include "math/utils.h"
#include "math/vec.h"
#include "objects/model.h"
#include "objects/physics_body.h"
#include "pool.h"
#include "shader.h"
#include "utils.h"

#include <stdbool.h>
#include <stdio.h>

#include <GLFW/glfw3.h>
#include <stdlib.h>

static int window_width = 1600, window_height = 900;
static int xoffset, yoffset;
static int lastx, lasty;

float interp_factor = 0.5;

#define GRAVITY 0.006f

bool piloting = false;
static long ship_idx;

typedef struct {
    PhysicsBody physics;
    Model model;
} Object;

typedef struct {
    unsigned int grid_vao;
    GLFWwindow* window;
} RenderCtx;

static PoolAllocator objects;

Mat4 camera_matrix(const Camera* cam) {
    Mat4 rot = quaternion_to_matrix(cam->actual_rotation);
    Vec3 inverted = cam->actual_position;
    inverted.x = -inverted.x;
    inverted.y = -inverted.y;
    inverted.z = -inverted.z;
    Mat4 trans = mat4_trans(inverted);

    return mat4_mult(rot, trans);
}

Vec3 camera_front(const Camera* cam) {
    return quaternion_rotate(cam->rotation, (Vec3) {.x = 0, .y = 0, .z = -1});
}

int get_window_width(void) {
    return window_width;
}

int get_window_height(void) {
    return window_height;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void) window;
    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    (void) window;
    xoffset = xpos - lastx;
    yoffset = ypos - lasty;

    lastx = xpos;
    lasty = ypos;
}

static int init(GLFWwindow** out_window) {
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    if (glfwInit() == GLFW_FALSE) {
        report_GLFW_error("glfwInit");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        report_GLFW_error("glfwCreateWindow");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        puts("Failed to initialize GLAD");
        return -1;
    }

    glViewport(0, 0, window_width, window_height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    *out_window = window;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    pool_init(&objects, 8192, sizeof(Object));

    init_input(window);
    return 0;
}

static void
draw_grid(GLuint program, GLuint vao, Mat4 proj_matrix, Mat4 view_matrix, const Camera* cam) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(vao);

    glUseProgram(program);

    shader_set_uniform_mat4(program, "u_view", &view_matrix);
    shader_set_uniform_mat4(program, "u_proj", &proj_matrix);
    shader_set_uniform_vec3(program, "u_cam_pos", cam->position);

    shader_set_uniform_float(program, "u_grid_size", 100);
    shader_set_uniform_float(program, "u_min_pixels_per_cell", 5);
    shader_set_uniform_float(program, "u_grid_cell_size", 1.0f);
    shader_set_uniform_vec4(
        program, "u_grid_color_thin", (Vec4) {.x = 0.5f, .y = 0.5f, .z = 0.5f, .w = 1});
    shader_set_uniform_vec4(
        program, "u_grid_color_thick", (Vec4) {.x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 1});

    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisable(GL_BLEND);
}

static void init_objects(void) {

    Object* ship = pool_alloc(&objects, &ship_idx);
    ship->model = (Model) {
        .transform =
            {
                        .scale = {.x = 1, .y = 3, .z = 1},
                        .position = {.z = 30},
                        .rotation = quaternion_unit(0, (Vec3) {.x = 1, .y = 0, .z = 0}),
                        },
        .mesh = generate_cubesphere(1, 2, (Vec3) {.y = 1}
             ),
    };

    ship->physics = (PhysicsBody) {
        .mass = 100,
    };
    init_model_buffers(&ship->model);
    physics_init_body(&ship->physics, &ship->model.mesh);

    Object* planet = pool_alloc(&objects, NULL);
    planet->model = (Model) {
        .mesh = generate_cubesphere(1, 30, (Vec3) {.y = 1}
             ),
        .transform =
            {
                                                   .scale = {.x = 10, .y = 10, .z = 10},
                                                   .rotation = quaternion_unit(0, (Vec3) {.x = 1, .y = 0, .z = 0}),
                                                   },
    };
    planet->physics = (PhysicsBody) {
        .mass = 20000,
    };
    init_model_buffers(&planet->model);
    physics_init_body(&planet->physics, &planet->model.mesh);
}

static void draw_object(void* obj_ptr, long idx, void* data) {
    (void) idx;
    Object* obj = obj_ptr;
    GLuint program = *(GLuint*) data;

    Mat4 model_matrix = transform_matrix(&obj->model.transform);

    shader_set_uniform_mat4(program, "gModel", &model_matrix);

    glBindVertexArray(obj->model.VAO);
    glDrawElements(GL_TRIANGLES, obj->model.mesh.index_count, GL_UNSIGNED_INT, NULL);

}

// TODO: Make shaders dynamic !
static void use_gpu(const Camera* cam, RenderCtx* ctx, GLuint basic_program, GLuint grid_program, double deltatime) {
    (void)deltatime;
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mat4 view_matrix = camera_matrix(cam);
    Mat4 proj_matrix =
        mat4_perpective(cam->actual_fov, (float) window_width / window_height, 0.1f, 5000.0f);

    glUseProgram(basic_program);

    shader_set_uniform_mat4(basic_program, "gView", &view_matrix);
    shader_set_uniform_mat4(basic_program, "gPerspective", &proj_matrix);
    shader_set_uniform_vec3(basic_program, "light_pos", (Vec3) {.z = 2000, .y = 500, .x = 600});
    shader_set_uniform_vec4(basic_program, "light_color", (Vec4) {1, 0.9, 0.7, 1.0});
    shader_set_uniform_vec4(basic_program, "ambient_color", (Vec4) {0.0, 0.0, 0.0, 0.0});

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    pool_foreach(&objects, &draw_object, &basic_program);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDisable(GL_CULL_FACE);
    draw_grid(grid_program, ctx->grid_vao, proj_matrix, view_matrix, cam);
    glEnable(GL_CULL_FACE);

    debug_draw(&view_matrix, &proj_matrix);
    glfwSwapBuffers(ctx->window);
}

typedef struct {
    long source_idx;
    Object* source;
} GravityInfo;

static void object_gravity(void* obj_ptr, long idx, void* data) {
    Object* target = obj_ptr;
    GravityInfo* info = data;

    if (info->source_idx == idx)
        return;

    Vec3 force =
        vec3_diff(info->source->model.transform.position, target->model.transform.position);
    float sqr_distance = vec3_sqrlength(force);
    force = vec3_normalize(force);
    vec3_mult(&force, GRAVITY * (target->physics.mass + info->source->physics.mass) / sqr_distance);

    physics_apply_force(&target->physics, force, VEC3_ZERO);
}

static void update_object_physics(void* obj_ptr, long idx, void* data) {
    (void) data;
    (void) idx;
    Object* obj = obj_ptr;
    double deltatime = *(double*)data;

    /*
    if (idx == 0) {
        physics_apply_local_force(
            &obj->physics, &obj->model.transform, (Vec3) {.y = 0.04}, (Vec3) {.y = -1});
        physics_apply_local_force(
            &obj->physics, &obj->model.transform, (Vec3) {.x = 0.0001}, (Vec3) {.y = 0.03});
    }
    */


    GravityInfo info = {
        .source = obj,
        .source_idx = idx,
    };
    pool_foreach(&objects, &object_gravity, &info);

    physics_update(&obj->physics, &obj->model.transform, deltatime);
    debug_add_arrow(obj->model.transform.position, obj->physics.velocity);
}

static void update_camera(Camera* cam) {

    if (piloting) {
        Object* obj = pool_get(&objects, ship_idx);
        Vec3 offset = {.y = -7, .z = 4};
        Quaternion rotation = quaternion_inv(obj->model.transform.rotation);
        Vec3 new_pos = vec3_add(obj->model.transform.position, quaternion_rotate(rotation, offset));
        cam->position = new_pos;
        cam->rotation =
            quaternion_mult(rotation, quaternion_unit(deg_to_rad(-45), (Vec3) {.x = 1}));
        ;
    } else {
        if (xoffset != 0) {
            cam->rotation = quaternion_mult(
                quaternion_unit(deg_to_rad(xoffset), (Vec3) {.x = 0, .y = 1, .z = 0}),
                cam->rotation);
            xoffset = 0;
        }

        if (yoffset != 0) {
            cam->rotation = quaternion_mult(
                quaternion_unit(deg_to_rad(yoffset),
                                quaternion_rotate(cam->rotation, (Vec3) {.x = 1, .y = 0, .z = 0})),
                cam->rotation);
            /*
            I was rotating the camera around the world X axis: If we turn 90° left or right, and
            we try to turn upwards, we roll instead... This was fixed by rotating around the X
            axis local to the camera, obtained by first rotating a unit vector (1,0,0) using the
            camera rotation.
            */
            yoffset = 0;
        }
    }

    cam->actual_position = vec3_lerp(cam->actual_position, cam->position, interp_factor);
    cam->actual_rotation = quaternion_lerp(cam->actual_rotation, cam->rotation, interp_factor);
    cam->actual_rotation = quaternion_normalize(cam->actual_rotation);
    cam->actual_fov = lerp(cam->actual_fov, cam->fov, interp_factor);
}

void use_cpu(RenderCtx* render_ctx, Camera* cam, double deltatime) {

    processInput(render_ctx->window, cam);

    /*
    Vec3 force = vec3_diff(sphere_model.transform.position, quad_model.transform.position);
    float distance = vec3_sqrlength(force);
    force = vec3_normalize(force);
    vec3_mult(&force, GRAVITY * (quad_body.mass + sphere_body.mass) / distance);
    physics_apply_force(&quad_body, force, VEC3_ZERO);
    physics_apply_force(&sphere_body, vec3_neg(force), VEC3_ZERO);
    */

    pool_foreach(&objects, update_object_physics, &deltatime);

    update_camera(cam);
}

int main(void) {
    RenderCtx render_ctx;
    int res = init(&render_ctx.window);
    if (res != 0)
        return res;

    glGenVertexArrays(1, &render_ctx.grid_vao);

    // glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    debug_init();

    GLuint basic_program;
    if (!shader_load("shaders/triangle.vsh", "shaders/triangle.fsh", &basic_program))
        return 1;

    GLuint grid_program;
    if (!shader_load("shaders/grid.vsh", "shaders/grid.fsh", &grid_program))
        return 1;

    Camera cam = {
        .rotation = quaternion_unit(deg_to_rad(90), (Vec3) {.x = 0, .y = 1, .z = 0}
            ),
        .position = {.z = 35, .y = 2},
        .fov = 90,
    };

    init_objects();

    double current_frame;
    double last_frame = glfwGetTime();

    while (!glfwWindowShouldClose(render_ctx.window)) {
        current_frame = glfwGetTime();
        double deltatime = current_frame - last_frame;

        //debug_add_arrow(VEC3_ZERO, (Vec3) {.x = 38, .y = 6, .z = -3});
        use_cpu(&render_ctx, &cam, deltatime);
        use_gpu(&cam, &render_ctx, basic_program, grid_program, deltatime);

        glfwPollEvents();
        last_frame = current_frame;
    }

    glDeleteProgram(basic_program);

    pool_destroy(&objects);

    debug_cleanup();

    glfwDestroyWindow(render_ctx.window);
    glfwTerminate();
    return 0;
}
