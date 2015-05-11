#version 330 core

// Input data
in vec3 Normal;
in vec3 FragEyePos;

// Output data
out vec4 Color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const int MAX_LIGHTS = 20;
uniform Light lights[MAX_LIGHTS];
uniform int num_lights;
uniform Material material;

uniform mat4 V;

void main()
{
    vec3 result = vec3(0.0, 0.0, 0.0);

    // Calculate lighting for each light source independently
    for (int i=0; i<num_lights; i++) {
        Light light = lights[i];

        /* Light dat biatch */
        // Ambient
        vec3 ambient = light.ambient * material.ambient;

        // Diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragEyePos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * (diff * material.diffuse);

        // Specular
        vec3 viewDir = normalize(- mat3(V)*FragEyePos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * material.specular);

        result += (ambient + diffuse + specular);
    }

    Color = vec4(result, 1.0f);
}