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
const float ATT_CONST = 0.2f;
const float ATT_LINEAR = 0.4f;
const float ATT_QUAD = 0.8f;

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
//uniform samplerCube depthCubemaps[MAX_POINT_LIGHTS];
uniform samplerCube depthCubemap0;
uniform samplerCube depthCubemap1;
uniform samplerCube depthCubemap2;
uniform samplerCube depthCubemap3;
uniform samplerCube depthCubemap4;
uniform samplerCube depthCubemap5;
uniform samplerCube depthCubemap6;
uniform samplerCube depthCubemap7;
uniform samplerCube depthCubemap8;
uniform samplerCube depthCubemap9;
uniform samplerCube depthCubemap10;
uniform samplerCube depthCubemap11;
uniform int numPointLights;

uniform float shadowMapping_far_plane;

uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];
uniform int numDirLights;

uniform Material material;

uniform mat4 V;
uniform mat4 V_inv;

uniform vec3 viewPos;

const vec3 NULL_VECTOR = vec3(0.0f, 0.0f, 0.0f);

float ShadowCalculation(vec3 fragToLight, int i)
{
    // Use the light to fragment vector to sample from the depth map
    float closestDepth = 0.1f;

    if (i == 0) {
        closestDepth = texture(depthCubemap0, fragToLight).r;
    } else if (i == 1) {
        closestDepth = texture(depthCubemap1, fragToLight).r;
    } else if (i == 2) {
        closestDepth = texture(depthCubemap2, fragToLight).r;
    } else if (i == 3) {
        closestDepth = texture(depthCubemap3, fragToLight).r;
    } else if (i == 4) {
        closestDepth = texture(depthCubemap4, fragToLight).r;
    } else if (i == 5) {
        closestDepth = texture(depthCubemap5, fragToLight).r;
    } else if (i == 6) {
        closestDepth = texture(depthCubemap6, fragToLight).r;
    } else if (i == 7) {
        closestDepth = texture(depthCubemap7, fragToLight).r;
    } else if (i == 8) {
        closestDepth = texture(depthCubemap8, fragToLight).r;
    } else if (i == 9) {
        closestDepth = texture(depthCubemap9, fragToLight).r;
    } else if (i == 10) {
        closestDepth = texture(depthCubemap10, fragToLight).r;
    } else if (i == 11) {
        closestDepth = texture(depthCubemap11, fragToLight).r;
    } /*else if (i == 12) {
        closestDepth = texture(depthCubemaps[12], fragToLight).r;
    } else if (i == 13) {
        closestDepth = texture(depthCubemaps[13], fragToLight).r;
    } else if (i == 14) {
        closestDepth = texture(depthCubemaps[14], fragToLight).r;
    } else if (i == 15) {
        closestDepth = texture(depthCubemaps[15], fragToLight).r;
    } else if (i == 16) {
        closestDepth = texture(depthCubemaps[16], fragToLight).r;
    } else if (i == 17) {
        closestDepth = texture(depthCubemaps[17], fragToLight).r;
    } else if (i == 18) {
        closestDepth = texture(depthCubemaps[18], fragToLight).r;
    } else {
        closestDepth = texture(depthCubemaps[19], fragToLight).r;
    } */
    // It is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= shadowMapping_far_plane;
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // Now test for shadows
    float bias = 0.05;
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec3 pointLightShading() {
    vec3 pointLightContribution = vec3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < numPointLights; i++) {
        PointLight light = pointLights[i];

        // Attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.0f / (ATT_CONST + ATT_LINEAR * distance + ATT_QUAD * (distance * distance));

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

        // Shadow calculations
        float shadow = ShadowCalculation(-lightDir, i);
        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

        // Returns 1.0f for all indices in range [0, numPointsLights], 0.0f otherwise
        float contrib = step( float(numPointLights), i );
        vec3 currentLightContrib = mix(NULL_VECTOR, lighting, contrib);
        pointLightContribution += currentLightContrib;
    }

    return pointLightContribution;
}

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);

    // Calculate contribution for all point lights in the scene
    result += pointLightShading();

    /*
    for (int i = 0; i < numDirLights; i++) {
        result += directionalLightShading(dirLights[i]);
    }
    */

    Color = vec4(result, 1.0f);
    //Color = vec4(FragPos.z, FragPos.z, FragPos.z, 1.0f);
}

/*
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
*/

/* TODO (bwiberg): implement spotLightShading
vec3 spotLightShading(Light spotLight) {
    return vec3(0.0, 0.0, 0.0);
}
*/
