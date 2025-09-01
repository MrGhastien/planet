#include "math/vec.h"
#include "mesh.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PHI 1.618033988749894

Mesh generate_icosphere(float radius, int resolution, Vec3 up) {
    float rect_height = sqrtf(1.0f / (1 + PHI));
    float rect_width = rect_height * PHI;
    (void) rect_width;
    (void) radius;
    (void) resolution;
    (void) up;

    Mesh mesh = {};

    return mesh;
}

static void compute_normals(Mesh* mesh) {
    mesh->normal_count = mesh->vertex_count;
    mesh->normals = malloc(sizeof *mesh->normals * mesh->normal_count);

    Vec3* normals = mesh->normals;
    const Vec3* vertices = mesh->vertices;
    const unsigned int* indices = mesh->indices;

    memset(normals, 0, sizeof *normals * mesh->normal_count);

    for(unsigned int i = 0; i < mesh->index_count; i += 3) {
        unsigned int a_idx = indices[i];
        unsigned int b_idx = indices[i + 1];
        unsigned int c_idx = indices[i + 2];

        Vec3 a_to_b = vec3_diff(vertices[b_idx], vertices[a_idx]);
        Vec3 a_to_c = vec3_diff(vertices[c_idx], vertices[a_idx]);
        Vec3 b_to_c = vec3_diff(vertices[c_idx], vertices[b_idx]);

        Vec3 a_normal = vec3_cross(a_to_b, a_to_c);
        Vec3 b_normal = vec3_cross(b_to_c, vec3_neg(a_to_b));
        Vec3 c_normal = vec3_cross(vec3_neg(a_to_c), vec3_neg(b_to_c));

        normals[a_idx] = vec3_add(normals[a_idx], a_normal);
        normals[b_idx] = vec3_add(normals[b_idx], b_normal);
        normals[c_idx] = vec3_add(normals[c_idx], c_normal);
    }

    for(unsigned int i = 0; i < mesh->normal_count; i++) {
        normals[i] = vec3_normalize(normals[i]);
    }
}

static void generate_cubesphere_face(
    float radius, int resolution, int face_index, Mesh* mesh, Vec3 up, Vec3 right, Vec3 forward) {
    int triangle_index = (resolution - 1) * (resolution - 1) * 6 * face_index;
    for (int x = 0; x < resolution; x++) {
        for (int y = 0; y < resolution; y++) {
            float right_coord = (x * 2.0f) / (resolution - 1) - 1;
            float forward_coord = (y * 2.0f) / (resolution - 1) - 1;

            int vertex_index = x * resolution + y + resolution * resolution * face_index;
            Vec3 point_on_cube = vec3_add(up, vec3_add(vec3_mult1(right, right_coord), vec3_mult1(forward, forward_coord)));
            mesh->vertices[vertex_index] = vec3_normalize(point_on_cube);
            vec3_mult(&mesh->vertices[vertex_index], radius);
            if (x < resolution - 1 && y < resolution - 1) {
                mesh->indices[triangle_index] = vertex_index;
                mesh->indices[triangle_index + 1] = vertex_index + resolution;
                mesh->indices[triangle_index + 2] = vertex_index + resolution + 1;
                mesh->indices[triangle_index + 3] = vertex_index;
                mesh->indices[triangle_index + 4] = vertex_index + resolution + 1;
                mesh->indices[triangle_index + 5] = vertex_index + 1;

                triangle_index += 6;
            }
        }
    }
}

Mesh generate_cubesphere(float radius, int resolution, Vec3 up) {

    Vec3 mup = up;
    vec3_mult(&mup, radius);

    Vec3 right = (Vec3) {.x = up.y, .y = up.z, .z = up.x};
    Vec3 forward = vec3_cross(up, right);

    Mesh mesh = {
        .vertex_count = resolution * resolution * 6,
        .index_count = (resolution - 1) * (resolution - 1) * 6 * 6,
        .normal_count = resolution * resolution * 6,
        .vertices = malloc(sizeof *mesh.vertices * resolution * resolution * 6),
        .indices = malloc(sizeof *mesh.indices * (resolution - 1) * (resolution - 1) * 6 * 6),
    };

    Vec3 ups[6] = {
        [0] = up,
        [1] = right,
        [3] = vec3_neg(right),
        [2] = vec3_neg(up),
        [4] = forward,
        [5] = vec3_neg(forward),
    };

    Vec3 rights[6] = {
        [0] = right,
        [1] = vec3_neg(up),
        [2] = vec3_neg(right),
        [3] = up,
        [4] = right,
        [5] = right,
    };

    Vec3 forwards[6] = {
        [0] = forward,
        [1] = forward,
        [2] = forward,
        [3] = forward,
        [4] = vec3_neg(up),
        [5] = up,
    };

    for (int i = 0; i < 6; i++) {
        generate_cubesphere_face(radius, resolution, i, &mesh, ups[i], rights[i], forwards[i]);
    }

    compute_normals(&mesh);

    /*
    printf("Vertices: %i\n", mesh.vertex_count);
    for(unsigned int i = 0; i < mesh.vertex_count; i++) {
        Vec3 v = mesh.vertices[i];
        printf("(%5f, %5f, %5f)\n", v.x, v.y, v.z);
    }

    printf("Indices: %i\n", mesh.index_count);
    for(unsigned int i = 0; i < mesh.index_count; i++) {
        printf("%i, ", mesh.indices[i]);
    }
    puts("");

    printf("Normals: %i\n", mesh.normal_count);
    for(unsigned int i = 0; i < mesh.normal_count; i++) {
        Vec3 v = mesh.normals[i];
        printf("(%25f, %25f, %25f)\n", v.x, v.y, v.z);
    }
    */

    return mesh;
}

Mesh generate_quad(float size, Vec3 up) {
    (void) size;
    (void) up;
    Vec3 vertices[] = {
        {.x = 0.5f,  .y = 0.5f,  .z = 0.0f}, // top right
        {.x = 0.5f,  .y = -0.5f, .z = 0.0f}, // bottom right
        {.x = -0.5f, .y = -0.5f, .z = 0.0f}, // bottom left
        {.x = -0.5f, .y = 0.5f,  .z = 0.0f}  // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0,
        1,
        3, // first triangle
        1,
        2,
        3 // second triangle
    };
    Mesh mesh = {
        .vertex_count = 4,
        .index_count = 6,
        .vertices = malloc(sizeof *mesh.vertices * mesh.vertex_count),
        .indices = malloc(sizeof *mesh.indices * mesh.index_count),
    };
    for (unsigned int i = 0; i < mesh.vertex_count; i++) {
        mesh.vertices[i] = vertices[i];
    }

    for (unsigned int i = 0; i < mesh.index_count; i++) {
        mesh.indices[i] = indices[i];
    }

    compute_normals(&mesh);
    return mesh;
}
