#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Window.h"
#include "Shader.h"

float vertices[] = {
  -0.5f, -0.5f,  0.0f,   1.0f, 0.0f, 0.0f,
   0.5f, -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,
   0.0f,  0.5f,  0.0f,   0.0f, 0.0f, 1.0f,
};

dg::Shader shader;
GLuint VAO;

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

void initScene() {
  // Create shader.
  shader = dg::Shader::FromFiles(
      "assets/shaders/shader.v.glsl", "assets/shaders/shader.f.glsl");

	// Create triangle vertices.
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO); 
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(
      shader.GetAttributeLocation("in_Position"),
      3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glVertexAttribPointer(
      shader.GetAttributeLocation("in_Color"),
      3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
}

void renderScene() {
  // Clear back buffer.
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw triangle.
  shader.Use();
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
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

  try {
    initScene();
  } catch (const std::exception& e) {
    std::cerr << "Failed to initialize scene: ";
    terminateWithError(e.what());
  }

  // Application loop.
  while(!window.ShouldClose()) {
    // Process input for current frame.
    processInput(window);

    window.StartRender();
    renderScene();
    window.FinishRender();

    // Poll events for next frame.
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

