#version 330 core

// Input - per vertex
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;   // Normal vector: a vector that is perpendicular to the vertex surface.

// Output - sent to fragment shader
out vec3 color;
out vec3 Normal;
out vec3 FragPos;

// Uniforms
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main(void) {
  color = abs(position.xyz);

  Normal = mat3(transpose(inverse(M))) * normal; //TODO: This should be done on the CPU and send it to the shaders via a uniform before drawing
  FragPos = vec3(M * vec4(position, 1.0f));
  gl_Position = P*V*M* vec4(position, 1.0f);
}
