#ifndef SHIP_H
#define SHIP_H

// Each ship has a main thruster, and 8 manoeuvre thrusters.
// There are 4 thrusters and the rear of the ship, 4 at the front, in each direction. (2 by direction)

/*
  Step 1: Activer / désactiver les propulseurs selon leur position et la direction dans laquelle ils poussent
  Step 2: Activer les bons propulseurs pour s'orienter et se déplacer vers une position.
 */

typedef struct {
    float reactor_thrust;
    float manoeuvre_thrust;
} ShipThrusters;

void ship_thrust(ShipThrusters* ship);

#endif /* ! SHIP_H */
