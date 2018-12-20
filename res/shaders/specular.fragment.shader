#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform sampler2D texture_reflect1;
uniform samplerCube skybox;

in vec2 TexCoords;
in vec3 Position;
in vec3 Normal;

layout (std140) uniform Flags
{
    bool diffuseEnabled;
    bool specularEnabled;
    bool heightEnabled;
    bool reflectEnabled;
};

void main()
{
    if (specularEnabled)
        FragColor = texture(texture_specular1, TexCoords);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
