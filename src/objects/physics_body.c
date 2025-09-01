#include "physics_body.h"
#include "math/quaternion.h"
#include "math/vec.h"
#include "math/utils.h"
#include <stdio.h>

static double compute_inertia_product(Vec3 a, Vec3 b, Vec3 c, unsigned int i, unsigned int j) {
    double values[3][3] = {
        {a.x, a.y, a.z},
        {b.x, b.y, b.z},
        {c.x, c.y, c.z},
    };

    return 2 * values[0][i] * values[0][j] + values[1][i] * values[2][j] + values[2][i] * values[1][j]
        + 2 * values[1][i] * values[1][j] + values[0][i] * values[2][j] + values[2][i] * values[0][j]
        + 2 * values[2][i] * values[2][j] + values[0][i] * values[1][j] + values[1][i] * values[0][j];
}

static double compute_inertia_moment(Vec3 a, Vec3 b, Vec3 c, unsigned int i) {
    double values[3][3] = {
        {a.x, a.y, a.z},
        {b.x, b.y, b.z},
        {c.x, c.y, c.z},
    };
    return (values[0][i] * values[0][i]) + values[1][i] * values[2][i]
        + (values[1][i] * values[1][i]) + values[0][i] * values[2][i]
        + (values[2][i] * values[2][i]) + values[0][i] * values[1][i];
}

void physics_compute_inertial(Mesh* mesh, double density, double* out_mass, Vec3* out_mass_center, Mat3* out_inertia_mat) {
    
    // TODO: understand all of this !

    double Ia = 0;
    double Ib = 0;
    double Ic = 0;
    double Iap = 0;
    double Ibp = 0;
    double Icp = 0;

    Vec3 mass_center = {0};
    double mass = 0;

    for(unsigned int i = 0; i < mesh->index_count; i+=3) {
        int idx_a = mesh->indices[i];
        int idx_b = mesh->indices[i + 1];
        int idx_c = mesh->indices[i + 2];
        Vec3 a = mesh->vertices[idx_a];
        Vec3 b = mesh->vertices[idx_b];
        Vec3 c = mesh->vertices[idx_c];


        double detj = vec3_dot(a, vec3_cross(b, c));
        double volume = detj / 6;
        double tet_mass = volume * density;
        Vec3 tet_mass_center = vec3_add(a, vec3_add(b, c));
        vec3_mult(&tet_mass_center, 0.25);

        Ia += detj * (compute_inertia_moment(a, b, c, 1) + compute_inertia_moment(a, b, c, 2));
        Ib += detj * (compute_inertia_moment(a, b, c, 0) + compute_inertia_moment(a, b, c, 2));
        Ic += detj * (compute_inertia_moment(a, b, c, 0) + compute_inertia_moment(a, b, c, 1));
        Iap += detj * compute_inertia_product(a, b, c, 1, 2);
        Ibp += detj * compute_inertia_product(a, b, c, 0, 1);
        Icp += detj * compute_inertia_product(a, b, c, 0, 2);

        mass_center = vec3_add(mass_center, vec3_mult1(tet_mass_center, tet_mass));
        mass += tet_mass;
    }
    vec3_mult(&mass_center, 1.0 / mass);
    // INCOMPLETE !
    Ia = density * Ia / 60.0 - mass * dsqr(mass_center.y + mass_center.z);
    Ib = density * Ib / 60.0 - mass * dsqr(mass_center.x + mass_center.z);
    Ic = density * Ic / 60.0 - mass * dsqr(mass_center.x + mass_center.y);
    Iap = density * Ia / 120.0 - mass * mass_center.y * mass_center.z;
    Ibp = density * Ib / 120.0 - mass * mass_center.x * mass_center.y;
    Icp = density * Ic / 120.0 - mass * mass_center.x * mass_center.z;


    Mat3 inertia_matrix = {
        .data = {
            [0] = Ia, [1] = -Ibp, [2] = -Icp,
            [3] = -Ibp, [4] = Ib, [5] = -Iap,
            [6] = -Icp, [7] = -Iap, [8] = Ic,
        },
    };
    *out_mass = mass;
    *out_mass_center = mass_center;
    *out_inertia_mat = inertia_matrix;
}

void physics_init_body(PhysicsBody* body, Mesh* mesh) {
    double mass;
    Vec3 mass_center;
    Mat3 inertia_mat;
    physics_compute_inertial(mesh, 1, &mass, &mass_center, &inertia_mat);
    
    body->inertia_inv = mat3_inverse(inertia_mat);
}

void physics_apply_force(PhysicsBody* body, Vec3 force, Vec3 global_offset) {
    if(body->locked)
        return;
    Vec3 accel = vec3_mult1(force, 1.0 / body->mass);
    body->velocity = vec3_add(body->velocity, accel);
    if(global_offset.x == 0 && global_offset.y == 0 && global_offset.z == 0)
        return;

    Vec3 torque = vec3_cross(global_offset, force);
    body->torque_acc = vec3_add(body->torque_acc, torque);
}

void physics_apply_local_force(PhysicsBody* body, const Transform* transform, Vec3 force, Vec3 local_offset) {
    Quaternion rotation_inverse = quaternion_inv(transform->rotation);
    Vec3 global_force = quaternion_rotate(rotation_inverse, force);
    Vec3 global_offset = quaternion_rotate(rotation_inverse, local_offset);
    physics_apply_force(body, global_force, global_offset);
}

void physics_update(PhysicsBody* body, Transform* transform, float delta_time) {
    if(body->locked)
        return;
    body->velocity = vec3_add(body->velocity, vec3_mult1(body->force_acc, delta_time / body->mass));
    transform->position = vec3_add(vec3_mult1(body->velocity, delta_time), transform->position);

    body->force_acc = (Vec3){0};
    
    body->angular_velocity = vec3_add(body->angular_velocity, mat3_vec_mult(body->inertia_inv, vec3_mult1(body->torque_acc, delta_time)));

    float rot_angle = vec3_length(body->angular_velocity) * delta_time;
    if(rot_angle != 0) {
        Vec3 rot_axis = vec3_normalize(body->angular_velocity);

        transform->rotation = quaternion_mult(transform->rotation, quaternion_unit(rot_angle, rot_axis));
    }

    body->torque_acc = (Vec3) {0};
}
