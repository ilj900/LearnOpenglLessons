#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
} fs_in;

struct Light {
    vec3 Position;
    vec3 Color;
};

uniform Light lights[4];
uniform sampler2D diffuseTexture;
uniform vec3 viewePos;

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);

    vec3 ambient = 0.0 * color;

    vec3 lighting = vec3(0.0);

    for (int i = 0; i < 4; i++)
    {
        vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lights[i].Color * diff * color;
        float distance = length(fs_in.FragPos - lights[i].Position);
        diffuse *= 1.0 / (distance * distance);
        lighting += diffuse;
    }

    FragColor = vec4(ambient + lighting, 1.0);
}
