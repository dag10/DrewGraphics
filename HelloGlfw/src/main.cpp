#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Window.h"

void processInput(dg::Window &window) {
  if(glfwGetKey(window.GetHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    window.SetShouldClose(true);
  }
}

void terminateWithError(const char *error) {
  std::cerr << error << std::endl;
  glfwTerminate();
  exit(-1);
}

int main() {
  // Print GLFW errors to stderr.
  glfwSetErrorCallback([](int code, const char *desc) {
    std::cerr << "GLFW Error: " << desc << std::endl;
  });

  if (!glfwInit()) {
    terminateWithError("Failed to initialize GLFW.");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // Create window.
  dg::Window window;
  try {
    window = dg::Window::Open(800, 600, "Hello OpenGL!");
  } catch (const std::exception& e) {
    terminateWithError(e.what());
  }
  
  // Load GLAD procedures.
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    terminateWithError("Failed to initialize GLAD.");
  }

  // Application loop.
  while(!window.ShouldClose()) {
    // Process input for current frame.
    processInput(window);

    window.StartRender();

    // Clear back buffer.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    window.FinishRender();

    // Poll events for next frame.
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

