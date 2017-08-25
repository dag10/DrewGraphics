#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Shader.h"

float vertices[] = {
  // positions          // tex coords
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
};

glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
  glm::vec3( 2.0f,  5.0f, -15.0f), 
  glm::vec3(-1.5f, -2.2f, -2.5f),  
  glm::vec3(-3.8f, -2.0f, -12.3f),  
  glm::vec3( 2.4f, -0.4f, -3.5f),  
  glm::vec3(-1.7f,  3.0f, -7.5f),  
  glm::vec3( 1.3f, -2.0f, -2.5f),  
  glm::vec3( 1.5f,  2.0f, -2.5f), 
  glm::vec3( 1.5f,  0.2f, -1.5f), 
  glm::vec3(-1.3f,  1.0f, -1.5f),
};

dg::Window window;
dg::Shader shader;
dg::Texture containerTexture;
dg::Texture awesomeFaceTexture;
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

  // Create textures.
  containerTexture = dg::Texture::FromPath("assets/textures/container.jpg");
  awesomeFaceTexture = dg::Texture::FromPath("assets/textures/awesomeface.png");

	// Create quad vertices.
  GLuint VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(
      shader.GetAttributeLocation("in_Position"),
      3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(
      shader.GetAttributeLocation("in_TexCoord"),
      2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
}

glm::mat4x4 create_cube_m_matrix(glm::vec3 cubePos, float angle) {
  glm::mat4x4 model(1.0f);
  model = glm::translate(model, cubePos);
  model = glm::rotate(
      model, (float)glfwGetTime() + glm::radians(angle),
      glm::vec3(0.5f, 1.0f, 0.0f));

  return model;
}

glm::mat4x4 create_pv_matrix() {
  glm::vec3 eyePos(0.0f, 2.0f, -6.0f);
  glm::mat4x4 view = glm::lookAt(
      eyePos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

  float fov = glm::radians(75.f);
  float aspect = window.GetWidth() / window.GetHeight();
  glm::mat4x4 projection = glm::perspective(
      glm::radians(60.0f), aspect, 0.1f, 100.0f);

  return projection * view;
}

void renderScene() {
  glm::mat4x4 pv = create_pv_matrix();

  // Clear back buffer.
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Render params.
  glBindVertexArray(VAO);
  glEnable(GL_DEPTH_TEST);
  
  // Set up cube material.
  shader.Use();
  shader.SetFloat("ELAPSED_TIME", glfwGetTime());
  shader.SetTexture(0, "MainTex", containerTexture);
  shader.SetTexture(1, "SecondaryTex", awesomeFaceTexture);

  // Render cubes.
  int numCubes = sizeof(cubePositions) / sizeof(cubePositions[0]);
  for (int i = 0; i < numCubes; i++) {
    glm::mat4x4 m = create_cube_m_matrix(cubePositions[i], 20.f * i);
    shader.SetMat4("MATRIX_MVP", pv * m);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
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

  return 0;
}

