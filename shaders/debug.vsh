#version 400 core
layout (location = 0) in vec3 aPos;

uniform mat4 gView;
uniform mat4 gPerspective;

out vec3 pos;

void main()
{
    vec4 vpos = vec4(aPos, 1.0);
    pos = aPos;
    gl_Position = gPerspective * gView * vpos;
}
