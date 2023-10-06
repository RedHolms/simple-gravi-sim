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

// Set to 1 to debug renderer
#define DEBUG_RENDERER 0

class GravitySimulation {
public:
  std::vector<Planet> planets;

  GLFWwindow* window = nullptr;

#if DEBUG_RENDERER
  // Disable simulation
  Ticker ticker = Ticker(0);
#else
  Ticker ticker = Ticker(SIMULATION_SPEED);
#endif

  Renderer* render = nullptr;

public:
  GravitySimulation() {
#if DEBUG_RENDERER
    planets = {
      Planet(Vector2(0.0, -300), 0, 200, 0xFF0000),
      Planet(Vector2(0.0, 400), 0, 400, 0x00FF00),
      Planet(Vector2(500, 300), 0, 100, 0x0000FF),
    };
#else
    planets = {
      Planet(Vector2(0.0, 0.0), 5E14, 50, 0xFFFFFFFF),
      Planet(Vector2(0.0, 600), 1E12, 20, 0xFFFF0000),
    };

    planets[1].punch(Vector2(6E12, 0));
#endif
  }

public:
  void init() {
    glfwInit();

    window = glfwCreateWindow(900, 900, "Gravity Simulation", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, onFramebufferSizeChanged);
  }

  void run() {
    printf("Tick rate: %d\n", SIMULATION_SPEED);

    double previous_frame_time = glfwGetTime();

    double fps_output_time = glfwGetTime() + 1.0;
    uint32_t frames_drawen = 0;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    gladLoadGL(glfwGetProcAddress);

#if DEBUG_RENDERER
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    render = new Renderer();

    render->init();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    render->setViewport(Vector2(width, height));

    while (!glfwWindowShouldClose(window)) {
      double deltaTime = glfwGetTime() - previous_frame_time;
      previous_frame_time = glfwGetTime();

      if (glfwGetTime() >= fps_output_time) {
        printf("FPS: %d (dt=%f)\n", frames_drawen, deltaTime);
        fps_output_time = glfwGetTime() + 1;
        frames_drawen = 0;
      }

      uint32_t ticks = ticker.tick();
      for (uint32_t i = 0; i < ticks; ++i) {
        tick();
      }

      render->beginFrame();
      draw();

#if DEBUG_RENDERER
      printf("Vertices: %llu; Indices: %llu\n", render->vertices.size(), render->indices.size());
#endif

      render->render();

      glfwPollEvents();
      glfwSwapBuffers(window);
      ++frames_drawen;
    }
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

  void draw() {
    for (Planet const& planet : planets) {
      Vector2 pos = planet.position;
      uint32_t color = planet.color;
      float radius = planet.radius;
    
      render->drawCircle(pos, radius, color);
    }
  }

public:
  static void onFramebufferSizeChanged(GLFWwindow* window, int width, int height) {
    GravitySimulation* app = (GravitySimulation*)glfwGetWindowUserPointer(window);

    app->render->setViewport(Vector2(width, height));
  }
};

int main() {
  GravitySimulation* app = new GravitySimulation();

  app->init();
  app->run();

  return 0;
}
