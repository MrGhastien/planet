#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gPerspective;

out vec3 normal;
out vec3 pos;

void main()
{
    vec4 vpos = gModel * vec4(aPos, 1.0);
    //normal = mat3(transpose(inverse(gModel))) * aNormal;
    normal = aNormal;
    pos = vpos.xyz;
    gl_Position = gPerspective * gView * vpos;
}
