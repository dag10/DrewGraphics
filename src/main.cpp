//
//  main.cpp
//

#if !defined(_OPENGL) & defined(_OPENGL)
#error "No graphics platform specified. Define either _OPENGL or _DIRECTX."
#endif

#if defined(_OPENGL)
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#endif

#if defined(_WIN32)
#include <Windows.h>
#endif

#include <EngineTime.h>
#include <Exceptions.h>
#include <Graphics.h>
#include <InputCodes.h>
#include <Window.h>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>

#include <scenes/CanvasTestScene.h>
#include <scenes/DeepCloningScene.h>
#include <scenes/MeshesScene.h>
#include <scenes/PortalScene.h>
#include <scenes/QuadScene.h>
#include <scenes/RobotScene.h>
#include <scenes/SimpleScene.h>
#include <scenes/TexturesScene.h>
#include <scenes/TutorialScene.h>
#include <scenes/VRScene.h>

using namespace dg;

static std::shared_ptr<dg::Window> window;

[[noreturn]] void terminateWithError(const std::string &error) {
  std::cerr << error << std::endl;
  if (window != nullptr) {
    window->Hide();
  }
  // If on Windows, make sure we show a minimized console window
  // and wait for the user to press enter before quitting.
#if defined(_MSC_VER)
  ShowWindow(GetConsoleWindow(), SW_RESTORE);
  std::cerr << std::endl;
  system("pause");
#endif
  Graphics::Shutdown();
  exit(-1);
}

std::unique_ptr<Scene> PromptForScene(
  const std::string& launchArg, std::string *chosenName = nullptr) {
  std::map<
    std::string,
    std::function<std::unique_ptr<dg::Scene>()>> constructors;
  constructors["portal"]     = dg::PortalScene::Make;
  constructors["tutorial"]   = dg::TutorialScene::Make;
  constructors["simple"]     = dg::SimpleScene::Make;
  constructors["cloning"]    = dg::DeepCloningScene::Make;
  constructors["cloning-vr"] = dg::DeepCloningScene::MakeVR;
  constructors["textures"]   = dg::TexturesScene::Make;
  constructors["meshes"]     = dg::MeshesScene::Make;
  constructors["meshes-vr"]  = dg::MeshesScene::MakeVR;
  constructors["robot"]      = dg::RobotScene::Make;
  constructors["robot-vr"]   = dg::RobotScene::MakeVR;
  constructors["quad"]       = dg::QuadScene::Make;
  constructors["canvas"]     = dg::CanvasTestScene::Make;
  constructors["vr"]         = dg::VRScene::Make;
  std::string sceneName;
  if (!launchArg.empty()) {
    sceneName = launchArg;
  } else {
#ifdef DEFAULT_SCENE
    sceneName = DEFAULT_SCENE;
#endif
  }
  while (constructors.find(sceneName) == constructors.end()) {
    if (!sceneName.empty()) {
      std::cerr << "Unknown scene \"" << sceneName << "\"." << std::endl
                << std::endl;
    }
    std::cout << "Available scenes:" << std::endl;
    for (auto iter = constructors.begin(); iter != constructors.end(); iter++) {
      std::cout << "\t" << iter->first << std::endl;
    }
    std::cout << std::endl
              << "Type \"exit\" to exit." << std::endl
              << std::endl
              << "Choose a scene: ";
    std::cin >> sceneName;
    if (std::cin.eof() || sceneName == "exit") {
      *chosenName = "";
      return nullptr;
    }
    std::cout << std::endl;
  }
  *chosenName = sceneName;
  return constructors[sceneName]();
}

#if defined(_WIN32)
static void FixCurrentDirectory() {
  // Get the real, full path to this executable, end the string before
  // the filename itself and then set that as the current directory
  char currentDir[1024] = {};
  GetModuleFileName(0, currentDir, 1024);
  char* lastSlash = strrchr(currentDir, '\\');
  if (lastSlash) {
    *lastSlash = '\0';
    SetCurrentDirectory(currentDir);
  }
}
#endif

#if defined(_OPENGL)
int main(int argc, const char* argv[]) {
#elif defined(_DIRECTX)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  Win32Window::CreateConsoleWindow(500, 120, 32, 120);
# if defined(_MSC_VER) & defined(_DEBUG)
  // Enable memory leak detection.
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
# endif
#endif

#if defined(_WIN32)
  FixCurrentDirectory();
#endif

  // Find intended scene.
  std::string sceneName;
#if defined(_OPENGL)
  std::string sceneArg = (argc > 1) ? std::string(argv[1]) : "";
#elif defined(_DIRECTX)
  std::string sceneArg = std::string(lpCmdLine);
#endif
  std::unique_ptr<Scene> scene = PromptForScene(sceneArg, &sceneName);
  if (scene == nullptr) {
    return 0;
  }

  // Create window.
  try {
#if defined(_OPENGL)
    window = dg::OpenGLWindow::Open(800, 600, "Drew Graphics");
#elif defined(_DIRECTX)
    window = dg::Win32Window::Open(800, 600, "Drew Graphics", hInstance);
#endif
  } catch (const EngineError& e) {
    terminateWithError(
        "Failed to open window: " + std::string(e.what()));
  }

  // Initialize graphics.
  try {
    Graphics::Initialize(*window);
  } catch (const EngineError& e) {
    terminateWithError(
        "Failed to initialize graphics: " + std::string(e.what()));
  }

  // Initialize scene.
  try {
    scene->SetWindow(window);
    scene->Initialize();
  } catch (const EngineError& e) {
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
    } catch (const EngineError& e) {
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
#if defined(_OPENGL)
        std::string platform = "OpenGL";
#elif defined(_DIRECTX)
        std::string platform = "DirectX";
#endif
        window->SetTitle(((std::ostringstream&)(std::ostringstream()
          << "Drew Graphics | "
          << platform << " | "
          << sceneName << " | "
          << (int)(1.0 / dg::Time::Delta) << " FPS | "
          << dg::Time::AverageFrameRate << " average FPS")).str());
      }
      lastWindowUpdateTime = dg::Time::Elapsed;
    }

    window->StartRender();
    try {
      scene->RenderFrame();
    } catch (const EngineError& e) {
      std::cerr << "Failed to render scene: ";
      terminateWithError(e.what());
    }
    window->FinishRender();
  }

  Graphics::Shutdown();
  window = nullptr;
  return 0;
}
