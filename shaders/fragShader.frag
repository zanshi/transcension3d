#version 330 core

// Ouput data
out vec3 color;

void main()
{
	// Output color = red
	float level = 1.0 - gl_FragCoord.z;
	color = vec3(0,level,0);

}
