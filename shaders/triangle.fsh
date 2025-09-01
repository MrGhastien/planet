#version 400 core
out vec4 FragColor;

uniform vec4 ambient_color;
uniform vec4 light_color;
uniform vec3 light_pos;

in vec3 normal;
in vec3 pos;

void main()
{
    vec3 light_dir = normalize(light_pos - pos);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse_light = diff * light_color.xyz;
    FragColor = vec4(ambient_color.xyz + diffuse_light, 1);

    // vec3 color_normal = (normal + vec3(1)) / 2;
    // FragColor = vec4(color_normal.xyz, 1);
} 
