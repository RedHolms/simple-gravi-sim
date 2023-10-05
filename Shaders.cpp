#include "Shaders.h"

const char* SHADER_VERTEX = R"GLSL(
#version 330 core

layout (location = 0) in vec2 dd_pos;
layout (location = 1) in vec3 dd_col;

out vec3 fr_col;

void main() {
  gl_Position = vec4(dd_pos, 0.0f, 1.0f);
  fr_col = dd_col;
}
)GLSL";

const char* SHADER_FRAGMENT = R"GLSL(
#version 330 core

in vec3 fr_col;

out vec4 FragColor;

void main() {
  FragColor = vec4(fr_col, 1.0f);
}
)GLSL";
