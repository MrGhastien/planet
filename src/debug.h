#ifndef DEBUG_H
#define DEBUG_H

#include "math/matrix.h"
#include "math/vec.h"

void debug_add_point(Vec3 pos);
void debug_add_arrow(Vec3 origin, Vec3 vec);

void debug_draw(Mat4* view_matrix, Mat4* proj_matrix);

void debug_init(void);
void debug_cleanup(void);

#endif /* ! DEBUG_H */
