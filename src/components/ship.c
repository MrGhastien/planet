#include "ship.h"
#include "physics_body.h"
#include <stdlib.h>

void ship_init(Ship* ship, Vec3 reactor_position, Vec3 reactor_force, unsigned int manoeuvre_count) {
    *ship = (Ship) {
        .reactor = {.thrust_position = reactor_position, .thrust_force = reactor_force},
        .manoeuvre_count = manoeuvre_count,
    };

    void* ptr = malloc(sizeof *ship->manoeuvre * manoeuvre_count);
    if (!ptr)
        abort();

    ship->manoeuvre = ptr;
}

/*
void ship_thrust(PhysicsBody* physics, Transform* transform, Ship* ship, int index) {
    if (index < 0)
        return;
    if (index == 0)
    physics_apply_local_force(physics, transform, Vec3 force, Vec3 local_offset)
}
*/
