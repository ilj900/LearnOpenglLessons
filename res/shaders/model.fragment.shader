#version 330 core
out vec4 FragColor;

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform sampler2D texture_reflect1;
uniform samplerCube skybox;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

in vec2 TexCoords;
in vec3 Position;
in vec3 Normal;

uniform vec3 cameraPos;

void main()
{
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect (I, normalize(Normal));
    vec4 reflectedColor = vec4(texture(skybox, R).rgb, 1.0);
    vec3 viewDir = normalize(cameraPos - Position);
    vec3 color = CalcDirLight(dirLight, Normal, viewDir);
    vec3 reflectValues = vec3(texture(texture_reflect1, TexCoords));
    FragColor = vec4(color, 1.0) + reflectedColor*(length(reflectValues));
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirLight.direction);
    normal = normalize(normal);
    float lightAngle = dot(normal, lightDir);
    float diff = max(lightAngle, 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float angle = dot(viewDir, reflectDir);
    float spec = pow(max(angle, 0.0), 32.0);
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}
