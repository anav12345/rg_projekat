//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform DirLight directional_light;
uniform PointLight red_point_light;
uniform PointLight yellow_point_light;
uniform PointLight green_point_light;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform float material_shininess;
uniform vec3 viewPosition;

uniform samplerCube depthMap;
uniform vec3 lightPos;
uniform float far_plane;


float ShadowCalculation(vec3 fragPos)
{

    vec3 fragToLight = fragPos - lightPos;

    float closestDepth = texture(depthMap, fragToLight).r;

    closestDepth *= far_plane;

    float currentDepth = length(fragToLight);

    float bias = 0.05;

    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;


    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse0, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse0, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular0, fs_in.TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse0, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse0, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular0, fs_in.TexCoords).xxx);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = ShadowCalculation(fs_in.FragPos);
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main()
{
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPosition - fs_in.FragPos);

    vec3 result = CalcDirLight(directional_light, normal, viewDir);
    result += CalcPointLight(red_point_light, normal, fs_in.FragPos, viewDir);
    result += CalcPointLight(yellow_point_light, normal, fs_in.FragPos, viewDir);
    result += CalcPointLight(green_point_light, normal, fs_in.FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}