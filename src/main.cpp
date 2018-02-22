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

using namespace dg;

[[noreturn]] void terminateWithError(const char *error) {
  std::cerr << error << std::endl;
#ifdef _OPENGL
  glfwTerminate();
#endif
  // If on Windows, make sure we show a minimized console window
  // and wait for the user to press enter before quitting.
#if defined(_MSC_VER)
  ShowWindow(GetConsoleWindow(), SW_RESTORE);
  std::cerr << std::endl;
  system("pause");
#endif
  exit(-1);
}

#if defined(_DIRECTX)
void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines,
                         int windowColumns) {
  CONSOLE_SCREEN_BUFFER_INFO coninfo;

  AllocConsole();
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
  coninfo.dwSize.Y = bufferLines;
  coninfo.dwSize.X = bufferColumns;
  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

  SMALL_RECT rect;
  rect.Left = 0;
  rect.Top = 0;
  rect.Right = windowColumns;
  rect.Bottom = windowLines;
  SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

  FILE* stream;
  freopen_s(&stream, "CONIN$", "r", stdin);
  freopen_s(&stream, "CONOUT$", "w", stdout);
  freopen_s(&stream, "CONOUT$", "w", stderr);

  // Prevent accidental console window closure.
  HWND consoleHandle = GetConsoleWindow();
  HMENU hmenu = GetSystemMenu(consoleHandle, FALSE);
  EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}
#endif

std::unique_ptr<Scene> PromptForScene(const std::string& launchArg) {
  std::map<
    std::string,
    std::function<std::unique_ptr<dg::Scene>()>> constructors;
  constructors["portal"]     = dg::PortalScene::Make;
  constructors["tutorial"]   = dg::TutorialScene::Make;
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
      return nullptr;
    }
    std::cout << std::endl;
  }
  return constructors[sceneName]();
}

#if defined(_OPENGL)
int main(int argc, const char* argv[]) {
#elif defined(_DIRECTX)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  CreateConsoleWindow(500, 120, 32, 120);
# if defined(_MSC_VER) & defined(_DEBUG)
  // Enable memory leak detection.
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
# endif
#endif

  std::string sceneArg;
#if defined(_OPENGL)
  if (argc > 1) {
    sceneArg = std::string(argv[1]);
  }
#elif defined(_DIRECTX)
  sceneArg = std::string(lpCmdLine);
#endif

  // Find intended scene.
  std::unique_ptr<Scene> scene = PromptForScene(sceneArg);
  if (scene == nullptr) {
    return 0;
  }

#ifdef _OPENGL
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
# ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
# endif
#endif

  // Create window.
  std::shared_ptr<dg::Window> window;
  try {
#if defined(_OPENGL)
    window = dg::OpenGLWindow::Open(800, 600, "Drew Graphics");
#elif defined(_DIRECTX)
    window = dg::Win32Window::Open(800, 600, "Drew Graphics", hInstance);
#endif
  } catch (const std::exception& e) {
    terminateWithError(e.what());
  }

#ifdef _OPENGL
  // Load GLAD procedures.
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    terminateWithError("Failed to initialize GLAD.");
  }
#endif

#if defined(_OPENGL) // TODO
  // Create primitive meshes.
  dg::Mesh::CreatePrimitives();
#endif

#if defined(_OPENGL) // TODO
  // Staticially initialize shader class.
  dg::Shader::Initialize();
#endif

#if defined(_OPENGL)
  // Configure global includes for all shader files.
  dg::Shader::SetVertexHead("assets/shaders/includes/vertex_head.glsl");
  dg::Shader::AddVertexSource("assets/shaders/includes/vertex_main.glsl");
  dg::Shader::SetFragmentHead("assets/shaders/includes/fragment_head.glsl");
  dg::Shader::AddFragmentSource("assets/shaders/includes/fragment_main.glsl");
#endif

  // Set up scene.
  try {
    scene->SetWindow(window);
#if defined(_OPENGL) // TODO
    scene->Initialize();
#endif
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
#if defined(_OPENGL) // TODO
      scene->Update();
#endif
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
#if defined(_OPENGL)
        std::string platform = "OpenGL";
#elif defined(_DIRECTX)
        std::string platform = "DirectX";
#endif
        window->SetTitle(((std::ostringstream&)(std::ostringstream()
              << "Drew Graphics | " << platform << " | "
              << (int)(1.0 / dg::Time::Delta) << " FPS | "
              << dg::Time::AverageFrameRate << " average FPS")).str());
      }
      lastWindowUpdateTime = dg::Time::Elapsed;
    }

    window->StartRender();
    try {
#if defined(_OPENGL) // TODO
      scene->RenderFrame();
#endif
    } catch (const std::exception& e) {
      std::cerr << "Failed to render scene: ";
      terminateWithError(e.what());
    }
    window->FinishRender();
  }

  return 0;
}
