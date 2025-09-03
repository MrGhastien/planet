#include "components/components.h"
#include "components/model.h"
#include "components/physics_body.h"
#include "components/ship.h"
#include "debug.h"
#include "pool.h"
#include "shader.h"

#include <string.h>

#define GRAVITY 0.3f

static size_t component_sizes[] = {
    [COMPONENT_MESH] = sizeof(Mesh),
    [COMPONENT_PHYSICS] = sizeof(PhysicsBody),
    [COMPONENT_SHIP] = sizeof(Ship),
    [COMPONENT_TRANSFORM] = sizeof(Transform),
    [COMPONENT_OPENGL] = sizeof(OpenGLBuffers),
};

static PoolAllocator component_pools[_COMPONENT_COUNT];
static PoolAllocator things;

void components_init(void) {

    pool_init(&things, 512, sizeof(Thing));

    for (int i = 0; i < _COMPONENT_COUNT; i++) {
        pool_init(&component_pools[i], 512, component_sizes[i]);
    }
}

void components_cleanup(void) {
    for (int i = 0; i < _COMPONENT_COUNT; i++) {
        pool_destroy(&component_pools[i]);
    }
    pool_destroy(&things);
}

long create_thing(void) {
    long idx;
    pool_alloc(&things, &idx);
    return idx;
}

void components_attach(long entity_id, enum ComponentType type, const void* data) {
    Thing* t = pool_get(&things, entity_id);

    long component_idx;
    void* component = pool_alloc(&component_pools[type], &component_idx);

    memcpy(component, data, component_sizes[type]);

    /* WARNING */
    *(long*) component = entity_id;

    t->components[type] = component_idx;
}

void* components_get(enum ComponentType type, long entity_id) {
    Thing* t = pool_get(&things, entity_id);
    return pool_get(&component_pools[type], t->components[type]);
}

typedef struct {
    long source_idx;
} GravityInfo;

static void object_gravity(void* obj_ptr, long idx, void* data) {
    Thing* target = obj_ptr;
    long source_id = *(long*)data;

    if (source_id == idx)
        return;

    Transform* source_transform = components_get(COMPONENT_TRANSFORM, source_id);
    PhysicsBody* source_physics = components_get(COMPONENT_PHYSICS, source_id);

    Transform* target_transform = pool_get(&component_pools[COMPONENT_TRANSFORM], target->components[COMPONENT_TRANSFORM]);
    PhysicsBody* target_physics = pool_get(&component_pools[COMPONENT_PHYSICS], target->components[COMPONENT_PHYSICS]);

    Vec3 force =
        vec3_diff(source_transform->position, target_transform->position);
    float sqr_distance = vec3_sqrlength(force);
    force = vec3_normalize(force);
    vec3_mult(&force, GRAVITY * (target_physics->mass + source_physics->mass) / sqr_distance);

    physics_apply_force(target_physics, force, VEC3_ZERO);
}

static void update_physics_components(void* ptr, long idx, void* data) {
    (void)idx;
    PhysicsBody* physics = ptr;
    double delta_time = *(double*)data;

    Transform* transform = components_get(COMPONENT_TRANSFORM, physics->entity_id);

    pool_foreach(&things, &object_gravity, &physics->entity_id);

    physics_update(physics, transform, delta_time);
    debug_add_arrow(transform->position, physics->velocity);
}

void components_update(double delta_time) {
    pool_foreach(&component_pools[COMPONENT_PHYSICS], &update_physics_components, &delta_time);
}

typedef struct {
    double delta_time;
    GLuint program;
} RenderCtx;

static void render_models(void* ptr, long idx, void* data) {
    (void)idx;
    OpenGLBuffers* buffers = ptr;

    RenderCtx* ctx = data;

    Mesh* mesh = components_get(COMPONENT_MESH, buffers->entity_id);
    Transform* transform = components_get(COMPONENT_TRANSFORM, buffers->entity_id);

    // TODO: Materials !
    Mat4 model_matrix = transform_matrix(transform);
    shader_set_uniform_mat4(ctx->program, "gModel", &model_matrix);

    draw_model(buffers, mesh);
}

void components_render(double delta_time, GLuint program) {
    RenderCtx ctx = {
        .delta_time = delta_time,
        .program = program,
    };
    pool_foreach(&component_pools[COMPONENT_OPENGL], &render_models, &ctx);
}
