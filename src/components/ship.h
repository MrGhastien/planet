#ifndef SHIP_H
#define SHIP_H

// Each ship has a main thruster, and 8 manoeuvre thrusters.
// There are 4 thrusters and the rear of the ship, 4 at the front, in each direction. (2 by direction)

/*
  Step 1: Activer / désactiver les propulseurs selon leur position et la direction dans laquelle ils poussent
  Step 2: Activer les bons propulseurs pour s'orienter et se déplacer vers une position.
 */

#include "math/vec.h"
#include "components/physics_body.h"
#include "components/components.h"

typedef struct {
    Vec3 thrust_position;
    Vec3 thrust_force;
} ShipThruster;    

typedef struct {
    long entity_id;
    ShipThruster reactor;
    ShipThruster* manoeuvre;
    unsigned int manoeuvre_count;
} Ship;

void ship_thrust(PhysicsBody* physics, Transform* transform, Ship* ship, int index);

#endif /* ! SHIP_H */
