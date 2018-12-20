#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Position;
out vec3 Normal;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


uniform mat4 model;
uniform mat3 NormalMatrix;

void main()
{
    TexCoords = aTexCoords;
    /// For some reason, some texture coordinates exceede 1.0, and it wasn't easy to find out...
    if (TexCoords.x > 1.0)
        TexCoords.x = TexCoords.x - 1.0;
    if (TexCoords.y > 1.0)
        TexCoords.y = TexCoords.x - 1.0;
    Normal = NormalMatrix * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Position = vec3(model * vec4(aPos, 1.0));
}
