//
//  main.cpp
//

#ifndef _OPENGL
#ifndef _DIRECTX
#error "No graphics platform specified. Define either _OPENGL or _DIRECTX."
#endif
#endif

#ifdef _OPENGL
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#include <Camera.h>
#include <EngineTime.h>
#include <InputCodes.h>
#include <Mesh.h>
#include <Shader.h>
#include <Window.h>
#include <scenes/CanvasTestScene.h>
#include <scenes/MeshesScene.h>
#include <scenes/PortalScene.h>
#include <scenes/QuadScene.h>
#include <scenes/RobotScene.h>
#include <scenes/TexturesScene.h>
#include <scenes/TutorialScene.h>
#include <scenes/DeepCloningScene.h>
#include <scenes/VRScene.h>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>

#ifdef _MSC_VER
  // This turns on memory leak detection on Windows.
  #define _CRTDBG_MAP_ALLOC 1
#endif

[[noreturn]] void terminateWithError(const char *error) {
  std::cerr << error << std::endl;
  glfwTerminate();
  // If on Windows, make sure we show a minimized console window
  // and wait for the user to press enter before quitting.
#ifdef _MSC_VER
  ShowWindow(GetConsoleWindow(), SW_RESTORE);
  std::cerr << std::endl;
  system("pause");
#endif
  exit(-1);
}

int main(int argc, const char *argv[]) {
  // Find intended scene.
  std::map<
    std::string,
    std::function<std::unique_ptr<dg::Scene>()>> constructors;
  constructors["portal"]    = dg::PortalScene::Make;
  constructors["tutorial"]  = dg::TutorialScene::Make;
  constructors["cloning"]   = dg::DeepCloningScene::Make;
  constructors["textures"]  = dg::TexturesScene::Make;
  constructors["meshes"]    = dg::MeshesScene::Make;
  constructors["meshes-vr"] = dg::MeshesScene::MakeVR;
  constructors["robot"]     = dg::RobotScene::Make;
  constructors["robot-vr"]  = dg::RobotScene::MakeVR;
  constructors["quad"]      = dg::QuadScene::Make;
  constructors["canvas"]    = dg::CanvasTestScene::Make;
  constructors["vr"]        = dg::VRScene::Make;
  std::string sceneName;
  if (argc > 1) {
    sceneName = argv[1];
  } else {
#ifdef DEFAULT_SCENE
    sceneName = DEFAULT_SCENE;
#endif
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
    std::cout << std::endl;
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

  // Minimize the console window on Windows.
#ifdef _MSC_VER
#ifndef DEFAULT_SCENE
  ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
#endif
#endif

  // Set up timing.
  dg::Time::Reset();
  double lastWindowUpdateTime = 0;

  // Application loop.
  bool cursorWasLocked = false;
  while(!window->ShouldClose()) {
    dg::Time::Update();
    window->PollEvents();
    try {
      scene->Update();
    } catch (const std::exception& e) {
      std::cerr << "Failed to update scene: ";
      terminateWithError(e.what());
    }

    // Handle escape key to release cursor or quit app.
    if (window->IsKeyJustPressed(dg::Key::ESCAPE) ||
        window->IsKeyJustPressed(dg::Key::Q)) {
      if (window->IsCursorLocked()) {
        cursorWasLocked = true;
        window->UnlockCursor();
      } else {
        window->SetShouldClose(true);
      }
    }

    // Handle click to regain cursor focus.
    if (cursorWasLocked && !window->IsCursorLocked() &&
        window->IsMouseButtonJustPressed(dg::BUTTON_LEFT)) {
      window->LockCursor();
    }

    // Update window title every 0.1 seconds.
    const float titleUpdateFreq = 0.1f;
    if (dg::Time::Elapsed > lastWindowUpdateTime + titleUpdateFreq) {
      if (scene->AutomaticWindowTitle()) {
        window->SetTitle(((std::ostringstream&)(std::ostringstream()
              << "Drew Graphics | " << sceneName << " | "
              << (int)(1.0 / dg::Time::Delta) << " FPS | "
              << dg::Time::AverageFrameRate << " average FPS")).str());
      }
      lastWindowUpdateTime = dg::Time::Elapsed;
    }

    window->StartRender();
    try {
      scene->RenderFrame();
    } catch (const std::exception& e) {
      std::cerr << "Failed to render scene: ";
      terminateWithError(e.what());
    }
    window->FinishRender();
  }

  return 0;
}

