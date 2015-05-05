#version 330 core

// Input data
in vec3 Normal;
in vec3 FragPos;

// Ouput data
out vec4 color;

// Uniforms
uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{

	vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (diffuse) * objectColor;
    color = vec4(result, 1.0f);

	float level = 1.0 - gl_FragCoord.z;
	//color = vec3(0,level,0);


	//color = vec3(1.0f,0.0f,0.0f);



}
