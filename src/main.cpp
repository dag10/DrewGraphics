#include <iostream>
#include <sstream>
#include <ostream>
#include <memory>
#include <map>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <EngineTime.h>
#include <Window.h>
#include <Camera.h>
#include <Shader.h>
#include <Mesh.h>
#include <scenes/TutorialScene.h>
#include <scenes/MeshesScene.h>
#include <scenes/PortalScene.h>
#include <scenes/QuadScene.h>
#include <scenes/VRScene.h>

[[noreturn]] void terminateWithError(const char *error) {
  std::cerr << error << std::endl;
  glfwTerminate();
  exit(-1);
}

int main(int argc, const char *argv[]) {
  // Find intended scene.
  std::map<
    std::string,
    std::function<std::unique_ptr<dg::Scene>()>> constructors;
  constructors["portal"]   = dg::PortalScene::Make;
  constructors["tutorial"] = dg::TutorialScene::Make;
  constructors["meshes"]   = dg::MeshesScene::Make;
  constructors["quad"]     = dg::QuadScene::Make;
  constructors["vr"]       = dg::VRScene::Make;
  std::string sceneName;
  if (argc > 1) {
    sceneName = argv[1];
  }
  while (constructors.find(sceneName) == constructors.end()) {
    if (!sceneName.empty()) {
      std::cerr << "Unknown scene \"" << sceneName << "\"." << std::endl << std::endl;
    }
    std::cout << "Available scenes:" << std::endl;
    for (auto iter = constructors.begin(); iter != constructors.end(); iter++) {
      std::cout << "\t" << iter->first << std::endl;
    }
    std::cout << std::endl << "Choose a scene: ";
    std::cin >> sceneName;
    if (std::cin.eof()) {
      exit(0);
    }
  }

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
  std::shared_ptr<dg::Window> window;
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

  // Staticially initialize shader class.
  dg::Shader::Initialize();

  // Configure global includes for all shader files.
  dg::Shader::SetVertexHead("assets/shaders/includes/vertex_head.glsl");
  dg::Shader::AddVertexSource("assets/shaders/includes/vertex_main.glsl");
  dg::Shader::SetFragmentHead("assets/shaders/includes/fragment_head.glsl");
  dg::Shader::AddFragmentSource("assets/shaders/includes/fragment_main.glsl");

  // Create scene.
  std::unique_ptr<dg::Scene> scene = constructors[sceneName]();
  try {
    scene->SetWindow(window);
    scene->Initialize();
  } catch (const std::exception& e) {
    std::cerr << "Failed to initialize scene: ";
    terminateWithError(e.what());
  }

  dg::Time::Reset();
  double lastWindowUpdateTime = 0;

  // Application loop.
  bool cursorWasLocked = false;
  while(!window->ShouldClose()) {
    dg::Time::Update();
    window->PollEvents();
    scene->Update();

    // Handle escape key to release cursor or quit app.
    if (window->IsKeyJustPressed(GLFW_KEY_ESCAPE) ||
        window->IsKeyJustPressed(GLFW_KEY_Q)) {
      if (window->IsCursorLocked()) {
        cursorWasLocked = true;
        window->UnlockCursor();
      } else {
        window->SetShouldClose(true);
      }
    }

    // Handle click to regain cursor focus.
    if (cursorWasLocked && !window->IsCursorLocked() &&
        window->IsMouseButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
      window->LockCursor();
    }

    // Update window title every 0.1 seconds.
    const float titleUpdateFreq = 0.1f;
    if (dg::Time::Elapsed > lastWindowUpdateTime + titleUpdateFreq) {
      window->SetTitle(((std::ostringstream&)(std::ostringstream()
            << "Drew Graphics | " << sceneName << " | "
            << (int)(1.0 / dg::Time::Delta) << " FPS | "
            << dg::Time::AverageFrameRate << " average FPS")).str());
      lastWindowUpdateTime = dg::Time::Elapsed;
    }

    window->StartRender();
    scene->RenderFrame();
    window->FinishRender();
  }

  return 0;
}

