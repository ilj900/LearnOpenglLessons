#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture;
uniform vec3 lightPos[4];
uniform vec3 viewPos;
uniform bool blinn;
uniform float gamma;

vec3 calcPointLight(vec3 lightPos, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 initialColor);

void main()
{
    vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;
    vec3 result = vec3(0.0);
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    for (int i = 0; i < 4; i++)
        result += calcPointLight(lightPos[i], normal, fs_in.FragPos, viewDir, color);
    FragColor = vec4(result, 1.0);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));

}

vec3 calcPointLight(vec3 lightPos, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 initialColor)
{
    vec3 ambient = 0.05 * initialColor;

    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * initialColor;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = vec3(0.3) * spec;
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    return (ambient + diffuse + specular) * attenuation;
}
