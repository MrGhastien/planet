#version 400 core

uniform mat4 u_view;
uniform mat4 u_proj;
uniform vec3 u_cam_pos;
uniform float u_grid_size;

out vec3 world_pos;

const vec3 vertices[4] = vec3[4](
                                 vec3(-1.0, 0.0,  -1.0),
                                 vec3(1.0,  0.0,  -1.0),
                                 vec3(1.0,  0.0,  1.0),
                                 vec3(-1.0, 0.0,  1.0)
                                 );

const int indices[6] = int[6](0, 2, 1, 0, 3, 2);

void main() {
    int idx = indices[gl_VertexID];
    vec3 vpos = vertices[idx] * u_grid_size;
    vpos.x += u_cam_pos.x;
    vpos.z += u_cam_pos.z;
    gl_Position = u_proj * u_view * vec4(vpos, 1.0);
    world_pos = vpos;
}
