#include <assert.h>
#include <stdio.h>
#include <math.h>

#include <vector>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Planet.h"
#include "Rendering.h"
#include "Ticker.h"
#include "Shaders.h"

#define G ( 6.67430151515e-11 ) // Gravity constant
#define SIMULATION_SPEED ( 40 ) // Ticks per second

class GravitySimulation {
public:
  std::vector<Planet> planets;

  GLFWwindow* window = nullptr;

  Ticker ticker = Ticker(SIMULATION_SPEED);

  Vector2 viewport;

  GLuint shaderProgram = GL_INVALID_VALUE;
  GLuint vertexShader = GL_INVALID_VALUE;
  GLuint fragmentShader = GL_INVALID_VALUE;

  GLuint VAO = GL_INVALID_VALUE;
  GLuint verticesBuffer = GL_INVALID_VALUE;
  GLuint indicesBuffer = GL_INVALID_VALUE;
  Vertices vertices;

public:
  GravitySimulation() {
    planets = {
      Planet(Vector2(0.0, 0.0), 1E14, 50, 0xFFFFFFFF),
      Planet(Vector2(0.0, 500), 1, 20, 0xFFFF0000),
    };

    planets[1].punch(Vector2(3, 0));
  }

public:
  void init() {
    glfwInit();

    window = glfwCreateWindow(1280, 720, "Gravity Simulation", nullptr, nullptr);
  }

  void run() {
    double previous_frame_time = glfwGetTime();

    double fps_output_time = glfwGetTime() + 1.0;
    uint32_t frames_drawen = 0;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    gladLoadGL(glfwGetProcAddress);

    initShaderPipeline();
    initBuffers();

    while (!glfwWindowShouldClose(window)) {
      double deltaTime = glfwGetTime() - previous_frame_time;
      previous_frame_time = glfwGetTime();

      ++frames_drawen;
      if (glfwGetTime() >= fps_output_time) {
        printf("FPS: %d (dt=%f)\n", frames_drawen, deltaTime);
        fps_output_time = glfwGetTime() + 1;
        frames_drawen = 0;
      }

      tick();
      render();

      glfwPollEvents();
      glfwSwapBuffers(window);
    }
  }

  void tryCompileShader(GLuint shader, const char* source) {
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char error_info[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(shader, 512, NULL, error_info);
      fprintf(stderr, "Failed to compile shader: %s\n", error_info);
    }
  }

  void initShaderPipeline() {
    shaderProgram = glCreateProgram();

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    tryCompileShader(vertexShader, SHADER_VERTEX);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    tryCompileShader(fragmentShader, SHADER_FRAGMENT);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char error_info[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, error_info);
      fprintf(stderr, "Failed to link shader: %s\n", error_info);
      fprintf(stderr, "Memory leak!!! If shaders were compiled successfully, they wont be cleaned up\n");
    }
  }

  void initBuffers() {
    glGenVertexArrays(1, &VAO);

    // Assert that indicesBuffer goes directly after verticesBuffer, so we can generate them with single glGenBuffers call
    assert(&verticesBuffer + 1 == &indicesBuffer && "Invalid memory structure");

    glGenBuffers(2, &verticesBuffer);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)( 2 * sizeof(float) ));
    glEnableVertexAttribArray(1);
  }

  void applyGravity(Planet& first, Planet& second) {
    Vector2 diff = second.position - first.position;

    Vector2 direction = diff.normalize();

    double distance = diff.length();
    double force = ( G * first.mass * second.mass ) / ( distance * distance );

    first.punch(direction * force);
    second.punch(-direction * force);
  }

  void gravityTick() {
    size_t planets_count = planets.size();

    for (size_t i = 0; i < planets_count; ++i) {
      for (size_t j = i + 1; j < planets_count; ++j) {
        applyGravity(planets[i], planets[j]);
      }
    }
  }

  void tick() {
    gravityTick();

    for (Planet& planet : planets) {
      planet.tick();
    }
  }

  void drawCircle(Vector2 center, float radius, uint32_t color, int num_segments = 15) {
    float r, g, b;

    r = (float)((color >> 16) & 0xFF) / 255;
    g = (float)((color >>  8) & 0xFF) / 255;
    b = (float)(color & 0xFF) / 255;

    float vert_ratio = viewport.y / viewport.x;

    constexpr float doublePI = MATH_PIf * 2;

    float cx = (float)center.x, cy = (float)center.y;

    float first_angle;
    float prev_angle;
    for (int segment = 0; segment < num_segments; segment++) {
      float angle = doublePI * ( (float)segment / num_segments );

      if (segment == 0) {
        first_angle = angle;
        prev_angle = angle;
        continue;
      }

      float x1 = cosf(angle) * radius * vert_ratio;
      float y1 = sinf(angle) * radius;

      float x2 = cosf(prev_angle) * radius * vert_ratio;
      float y2 = sinf(prev_angle) * radius;

      prev_angle = angle;

      vertices.addVertex({ cx, cy, r, g, b });
      vertices.addVertex({ cx + x1, cy + y1, r, g, b });
      vertices.addVertex({ cx + x2, cy + y2, r, g, b });
    }

    float x1 = cosf(first_angle) * radius * vert_ratio;
    float y1 = sinf(first_angle) * radius;

    float x2 = cosf(prev_angle) * radius * vert_ratio;
    float y2 = sinf(prev_angle) * radius;

    vertices.addVertex({ cx, cy, r, g, b });
    vertices.addVertex({ cx + x1, cy + y1, r, g, b });
    vertices.addVertex({ cx + x2, cy + y2, r, g, b });
  }

  void draw() {
    for (Planet const& planet : planets) {
      Vector2 pos = planet.position;
      uint32_t color = planet.color;
      float radius = (float)planet.radius;

      pos.x /= viewport.x;
      pos.y /= viewport.y;
      radius /= viewport.y;

      drawCircle(pos, radius, color, 40);
    }
  }

  void render() {
    int width, height;

    glfwMakeContextCurrent(window);

    glfwGetFramebufferSize(window, &width, &height);
    viewport = { (float)width, (float)height };
    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    draw();

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glBufferData(GL_ARRAY_BUFFER, vertices.vertices.size() * sizeof(Vertex), vertices.vertices.data(), GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices.indices.size() * sizeof(uint16_t), vertices.indices.data(), GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLES, vertices.indices.size(), GL_UNSIGNED_SHORT, NULL);

    vertices.clear();
  }
};

int main() {
  GravitySimulation* app = new GravitySimulation();

  app->init();
  app->run();

  return 0;
}
