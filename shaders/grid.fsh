#version 400 core

uniform float u_grid_cell_size;
uniform float u_grid_size;
uniform vec4 u_grid_color_thin;
uniform vec4 u_grid_color_thick;

uniform float u_min_pixels_per_cell;
uniform vec3 u_cam_pos;

layout (location = 0) out vec4 frag_color;

in vec3 world_pos;

float map(float x, float old_min, float old_max, float new_min, float new_max) {
    return (x - old_min) * (new_max - new_min) / (old_max - old_min) + new_min;
}

float log10(float x) {
    return log(x) / log(10);
}

float compute_alpha(float cell_size, vec2 world_pos_derivatives) {
    vec2 mod_div = mod(world_pos.xz, cell_size) / world_pos_derivatives;
    vec2 alphas = vec2(1) - abs(clamp(mod_div, 0, 1) * 2 - vec2(1));
    return max(alphas.x, alphas.y);
}

void main() {

    vec2 dx = vec2(dFdx(world_pos.x), dFdy(world_pos.x));
    vec2 dz = vec2(dFdx(world_pos.z), dFdy(world_pos.z));

    float lx = length(dx);
    float lz = length(dz);

    vec2 l = vec2(lx, lz) * 2;

    float lod = max(0, log10(length(l) * u_min_pixels_per_cell / u_grid_cell_size));
    float actual_cell_size = u_grid_cell_size * pow(10, floor(lod));
    float big_cell_size = actual_cell_size * 10;
    float huge_cell_size = big_cell_size * 10;

    float lod_fade = fract(lod);

    float alpha = compute_alpha(actual_cell_size, l);
    float big_alpha = compute_alpha(big_cell_size, l);
    float huge_alpha = compute_alpha(huge_cell_size, l);

    vec4 color;
    if(huge_alpha > 0) {
        color = u_grid_color_thick;
        alpha = huge_alpha;
    } else if(big_alpha > 0) {
        color = mix(u_grid_color_thick, u_grid_color_thin, lod_fade);
        alpha = big_alpha;
    } else {
        color = u_grid_color_thin;
        alpha = alpha * (1 - lod_fade);
    }

    float falloff = 1 - clamp(length(world_pos.xz - u_cam_pos.xz) / u_grid_size, 0, 1);

    frag_color = vec4(color.rgb, color.a * alpha * falloff);
}
