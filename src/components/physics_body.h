#ifndef ATTRACTOR_H
#define ATTRACTOR_H

#include "components.h"
#include <stdbool.h>

typedef struct {
    long entity_id;
    float mass;
    Vec3 velocity;
    Vec3 angular_velocity;

    Vec3 force_acc;
    Vec3 torque_acc;

    Mat3 inertia_inv;
    bool locked;
} PhysicsBody;


void physics_apply_force(PhysicsBody* body, Vec3 force, Vec3 global_offset);
// Takes into account the rotation of the body
void physics_apply_local_force(PhysicsBody* body, const Transform* transform, Vec3 force, Vec3 local_offset);
void physics_init_body(PhysicsBody* body, Mesh* mesh);

void physics_update(PhysicsBody* body, Transform* transform, float delta_time);

#endif /* ! ATTRACTOR_H */
