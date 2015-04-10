#version 330 core

in vec3 inputColor;
in vec4 inputPosition;

out vec3 color;

void main(void) {
  color = abs(inputPosition.xyz);
  gl_Position = inputPosition;
}
