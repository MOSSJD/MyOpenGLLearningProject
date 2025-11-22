#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal; 
in vec3 fragPos;
in vec2 textureCoordinate;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec4 fragColor;

void main()
{
    // Light strength
    float dist = distance(viewPos, fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, textureCoordinate));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, textureCoordinate));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, textureCoordinate));
    // vec3 specular = vec3(0.0f);

    vec3 result = attenuation * (ambient + diffuse + specular);
    fragColor = vec4(result, 1.0);
}  