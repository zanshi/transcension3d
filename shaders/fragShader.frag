#version 330 core

// Input data
in vec3 Normal;
in vec3 FragPos;

// Output data
out vec4 Color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

const int POINT = 0;
const int DIRECTIONAL = 1;
const int SPOT = 2;

struct Light {
    // Light types:
    // 0: POINT
    // 1: DIRECTIONAL
    // 2: SPOT
    int type;

    // Position and direction vectors in world coordinates
    vec3 position;
    vec3 direction;

    // Lighting attributes for shading
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const int MAX_LIGHTS = 20;
uniform Light lights[MAX_LIGHTS];
uniform int num_lights;
uniform Material material;

uniform mat4 V;
uniform mat4 V_inv;

uniform vec3 viewPos;

vec3 pointLightShading(Light pointLight) {
    // Ambient
    vec3 ambient = pointLight.ambient * material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * (diff * material.diffuse);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = pointLight.specular * (spec * material.specular);

    return ambient+diffuse+specular;
}

vec3 directionalLightShading(Light dirLight) {
    // Ambient
    vec3 ambient = dirLight.ambient * material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = -dirLight.direction;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * (diff * material.diffuse);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = dirLight.specular * (spec * material.specular);

    return ambient+diffuse+specular;
}

vec3 spotLightShading(Light spotLight) {
    return vec3(0.0, 0.0, 0.0);
}

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);

    // Calculate lighting for each light source independently
    for (int i = 0; i < num_lights; i++) {
        Light light = lights[i];

        // TODO (bwiberg): Get rid of branching, i.e. clean up the light type handling with mix, clamp etc...
        // Light according to the light type
        switch (light.type) {
            case POINT:
                result += pointLightShading(light);
                break;
            case DIRECTIONAL:
                result += directionalLightShading(light);
                break;
            case SPOT:
                result += spotLightShading(light);
                break;
            default:
                // do nothing;
        }
    }

    Color = vec4(result, 1.0f);
}