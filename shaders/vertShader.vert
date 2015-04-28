#version 330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

in vec3 inputColor;
in vec4 inputPosition;

out vec3 color;

void main(void) {
  color = abs(inputPosition.xyz);

  gl_Position = P*V*M*inputPosition;
}
