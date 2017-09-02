#include <iostream>
#include <map>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "TutorialScene.h"
#include "PortalScene.h"

dg::Window window;

void processInput(dg::Window &window) {
  if(glfwGetKey(window.GetHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    window.SetShouldClose(true);
  }
}

[[noreturn]] void terminateWithError(const char *error) {
  std::cerr << error << std::endl;
  glfwTerminate();
  exit(-1);
}

int main(int argc, const char *argv[]) {
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
    window = dg::Window::Open(800, 600, "Drew Graphics");
  } catch (const std::exception& e) {
    terminateWithError(e.what());
  }
  
  // Load GLAD procedures.
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    terminateWithError("Failed to initialize GLAD.");
  }
  
  // Create primitive meshes.
  dg::Mesh::CreatePrimitives();

  std::map<
    std::string,
    std::function<std::unique_ptr<dg::Scene>()>> constructors;
  constructors["portal"]   = dg::PortalScene::Make;
  constructors["tutorial"] = dg::TutorialScene::Make;

  // Find intended scene.
  if (argc < 2) {
    terminateWithError("Specify a scene.");
  }
  if (constructors.find(argv[1]) == constructors.end()) {
    terminateWithError("Unknown scene.");
  }
  std::unique_ptr<dg::Scene> scene = constructors[argv[1]]();

  // Create scene.
  try {
    scene->Initialize();
  } catch (const std::exception& e) {
    std::cerr << "Failed to initialize scene: ";
    terminateWithError(e.what());
  }

  // Application loop.
  while(!window.ShouldClose()) {
    scene->Update();

    // Process input for current frame.
    processInput(window);

    window.StartRender();
    scene->Render(window);
    window.FinishRender();

    // Poll events for next frame.
    glfwPollEvents();
  }

  return 0;
}

