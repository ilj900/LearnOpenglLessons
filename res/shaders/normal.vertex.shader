#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat3 NormalMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.normal = normalize(vec3(projection * vec4(NormalMatrix * aNormal, 0.0)));
}
