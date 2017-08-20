#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "Hello OpenGL";

void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
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
  GLFWwindow *window = glfwCreateWindow(
     WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
  if (window == nullptr) {
    terminateWithError("Failed to create GLFW window.");
  }
  glfwMakeContextCurrent(window);
  
  // Load GLAD procedures.
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    terminateWithError("Failed to initialize GLAD.");
  }

  // Specify the rect to render to within the window.
  // TODO: Update this when we detect a window resize event.
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  // Application loop.
  while(!glfwWindowShouldClose(window)) {
    // Process input for current frame.
    processInput(window);

    // Clear back buffer.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Present back buffer.
    glfwSwapBuffers(window);

    // Poll events for next frame.
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

