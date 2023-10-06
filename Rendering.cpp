#include "Rendering.h"

#include <glad/gl.h>

#include "Shaders.h"

Shaders::Shaders() {
  program = glCreateProgram();

  for (GLuint& shader : shaders) {
    shader = -1;
  }
}

Shaders::~Shaders() {
  for (GLuint shader : shaders) {
    glDetachShader(program, shader);
    glDeleteShader(shader);
  }

  glDeleteProgram(program);
}

bool Shaders::compile(GLenum shaderType, const char* code) {
  uint32_t shaderIndex = -1;

  for (uint32_t i = 0; i < shaders.size(); ++i) {
    if (shaders[i] == -1) {
      shaderIndex = i;
      break;
    }
  }

  if (shaderIndex == -1) {
    lastError = "No slot for shader";
    return false;
  }

  GLuint shader = glCreateShader(shaderType);

  glShaderSource(shader, 1, &code, NULL);
  glCompileShader(shader);

  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    char error_info[512];

    glGetShaderInfoLog(shader, sizeof(error_info), NULL, error_info);
    lastError = error_info;

    glDeleteShader(shader);

    return false;
  }
  
  glAttachShader(program, shader);
  shaders[shaderIndex] = shader;

  return true;
}

bool Shaders::link() {
  glLinkProgram(program);

  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    char error_info[512];

    glGetProgramInfoLog(program, sizeof(error_info), NULL, error_info);
    lastError = error_info;

    return false;
  }

  return true;
}

void Shaders::select() {
  glUseProgram(program);
}

Renderer::Renderer() {
  vertexArray = -1;
  verticesBuffer = -1;
  indicesBuffer = -1;
}

void Renderer::init() {
  initShaders();
  initBuffers();
}

void Renderer::initShaders() {
  if (!shaders.compile(GL_VERTEX_SHADER, SHADER_VERTEX)) {
    fprintf(stderr, "Failed to compile vertex shader: %s\n", shaders.lastError.c_str());
  }

  if (!shaders.compile(GL_FRAGMENT_SHADER, SHADER_FRAGMENT)) {
    fprintf(stderr, "Failed to compile fragment shader: %s\n", shaders.lastError.c_str());
  }

  if (!shaders.link()) {
    fprintf(stderr, "Failed to link shaders: %s\n", shaders.lastError.c_str());
  }
}

void Renderer::initBuffers() {
  glGenVertexArrays(1, &vertexArray);

  glGenBuffers(1, &verticesBuffer);
  glGenBuffers(1, &indicesBuffer);

  glBindVertexArray(vertexArray);

  glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

  // Position
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
  glEnableVertexAttribArray(0);

  // Color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));
  glEnableVertexAttribArray(1);
}

void Renderer::setViewport(Vector2 viewport) {
  glViewport(0, 0, viewport.x, viewport.y);
  this->viewport = viewport;
}

void Renderer::beginFrame() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::render() {
  shaders.select();

  glBindVertexArray(vertexArray);

  glBufferData(GL_ARRAY_BUFFER,         vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Index), indices.data(), GL_DYNAMIC_DRAW);

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

  clearBuffers();
}

void Renderer::clearBuffers() {
  vertices.clear();
  indices.clear();
}

void Renderer::drawCircle(Vector2 center, float radius, uint32_t colorRGB, uint32_t segments_number) {
  float vert_ratio = viewport.y / viewport.x;

  center.x /= viewport.x;
  center.y /= viewport.y;
  radius /= viewport.y;

  float segment_angle = DOUBLE_PI * 1.0f / segments_number;

  Index center_vertex = addNewVertex({ center, colorRGB });
  Index start_vertex = addNewVertex({ center + Vector2(radius, 0), colorRGB });

  Index prev_vertex = start_vertex;

  for (uint32_t segment = 0; segment < segments_number; ++segment) {
    float angle = segment_angle * (segment + 1);

    float x = cosf(angle) * radius;
    float y = sinf(angle) * radius;

    Index new_vertex = addNewVertex({ center + Vector2(x, y), colorRGB });

    addIndex(center_vertex);
    addIndex(prev_vertex);
    addIndex(new_vertex);

    prev_vertex = new_vertex;
  }
}

void Renderer::drawPoligon(Vector2 p1, Vector2 p2, Vector2 p3, uint32_t colorRGB) {
  addVertex({ p1, colorRGB });
  addVertex({ p2, colorRGB });
  addVertex({ p3, colorRGB });
}

Index Renderer::addNewVertex(Vertex vertex) {
  vertices.push_back(vertex);
  return vertices.size() - 1;
}

Index Renderer::addVertex(Vertex vertex) {
  for (uint32_t i = 0; i < vertices.size(); ++i) {
    if (vertices[i] == vertex) {
      addIndex(i);
      return i;
    }
  }

  Index index = addNewVertex(vertex);
  addIndex(index);

  return index;
}

void Renderer::addIndex(Index index) {
  indices.push_back(index);
}
