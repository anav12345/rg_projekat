//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model)))*aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
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

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform DirLight directional_light;
uniform PointLight red_point_light;
uniform PointLight yellow_point_light;
uniform PointLight green_point_light;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform float material_shininess;
uniform vec3 viewPosition;

uniform samplerCube depthMap;
uniform float far_plane;


vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);


float ShadowCalculation(vec3 fragPos, vec3 lightPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
        float bias = 0.15;
        int samples = 20;
        float viewDistance = length(viewPosition - fragPos);
        float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
        for(int i = 0; i < samples; ++i)
        {
            float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
            closestDepth *= far_plane;
            if(currentDepth - bias > closestDepth)
                shadow += 1.0;
        }
        shadow /= float(samples);

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
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse0, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse0, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular0, TexCoords));
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
    // shadow calculation
    float shadow = ShadowCalculation(fragPos, light.position);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse0, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse0, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular0, TexCoords).xxx);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    ambient *= (1.0 - shadow * 0.7);
    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);

    vec3 result = CalcDirLight(directional_light, normal, viewDir);
    result += CalcPointLight(red_point_light, normal, FragPos, viewDir);
    result += CalcPointLight(yellow_point_light, normal, FragPos, viewDir);
    result += CalcPointLight(green_point_light, normal, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}