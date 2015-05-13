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

const int MAX_POINT_LIGHTS = 20;
struct PointLight {
    // Position vector in world coordinates
    vec3 position;
    // Lighting attributes for shading
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const int MAX_DIR_LIGHTS = 20;
struct DirectionalLight {
    // Position and direction vectors in world coordinates
    vec3 position;
    vec3 direction;
    // Lighting attributes for shading
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

/* TODO (bwiberg): Implement SpotLights
struct SpotLight {}
*/

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];
uniform int numDirLights;

uniform Material material;

uniform mat4 V;
uniform mat4 V_inv;

uniform vec3 viewPos;

vec3 pointLightShading(PointLight light) {
    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    return ambient+diffuse+specular;
}

vec3 directionalLightShading(DirectionalLight light) {
    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);

    // TODO (bwiberg): Should use '-light.direction', but is set to (0,0,0) so use position instead
    vec3 lightDir = normalize(light.position);
    //vec3 lightDir = -light.direction;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    return ambient+diffuse+specular;
}

/* TODO (bwiberg): implement spotLightShading
vec3 spotLightShading(Light spotLight) {
    return vec3(0.0, 0.0, 0.0);
}
*/

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);

    // Calculate shading for all lights of each type
    for (int i = 0; i < numPointLights; i++) {
        result += pointLightShading(pointLights[i]);
    }

    for (int i = 0; i < numDirLights; i++) {
        result += directionalLightShading(dirLights[i]);
    }

    Color = vec4(result, 1.0f);
}