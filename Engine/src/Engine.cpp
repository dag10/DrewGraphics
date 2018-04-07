//
//  Engine.cpp
//

#include "dg/Engine.h"
#include "dg/Scene.h"
#include "dg/Utils.h"
#include "dg/Window.h"

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <unistd.h>
#endif

dg::BaseEngine *dg::BaseEngine::instance = nullptr;

dg::BaseEngine::BaseEngine(std::shared_ptr<Window> window) : window(window) {
  assert(window != nullptr);
  assert(instance == nullptr);
  instance = this;
}

dg::BaseEngine::~BaseEngine() {
  Graphics::Shutdown();
  instance = nullptr;
}

void dg::BaseEngine::Initialize() {
  FixCurrentDirectory();

  try {
    Graphics::Initialize(*window);
  } catch (const EngineError& e) {
    throw std::runtime_error("Failed to initialize graphics: " +
                             std::string(e.what()));
  }
}

void dg::BaseEngine::StartScene(std::shared_ptr<Scene> scene) {
  nextScene = scene;
}

void dg::BaseEngine::Update() {
  if (nextScene != nullptr) {
    StartNextScene();
  }

  if (scene == nullptr) {
    return;
  }

  dg::Time::Update();
  window->PollEvents();

  try {
    scene->Update();
  } catch (const EngineError &e) {
    throw std::runtime_error("Failed to update scene: " +
                             std::string(e.what()));
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

  UpdateWindowTitle();

  // Render.
  window->StartRender();
  try {
    scene->RenderFrame();
  } catch (const EngineError &e) {
    throw std::runtime_error("Failed to render scene: " +
                             std::string(e.what()));
  }
  window->FinishRender();
}

void dg::BaseEngine::UpdateWindowTitle() {
  // Update window title every 0.1 seconds.
  const float titleUpdateFreq = 0.1f;
  if (dg::Time::Elapsed < lastWindowUpdateTime + titleUpdateFreq) {
    return;
  }

  if (scene->AutomaticWindowTitle()) {
    window->SetTitle(
        ((std::ostringstream &)(std::ostringstream()
                                << "Drew Graphics | " << Graphics::GetAPIName()
                                << " | " << (int)(1.0 / dg::Time::Delta)
                                << " FPS | " << dg::Time::AverageFrameRate
                                << " average FPS"))
            .str());
  }
  lastWindowUpdateTime = dg::Time::Elapsed;
}

bool dg::BaseEngine::ShouldQuit() const {
  if (window == nullptr) {
    return true;
  }

  return window->ShouldClose();
}

void dg::BaseEngine::StartNextScene() {
  if (scene != nullptr) {
    scene = nullptr;
  }

  if (nextScene == nullptr) {
    return;
  }

  scene = nextScene;
  nextScene = nullptr;

  try {
    scene->SetWindow(window);
    scene->Initialize();
  } catch (const EngineError& e) {
    throw std::runtime_error("Failed to initialize scene: " +
                             std::string(e.what()));
  }

  // Set up timing.
  dg::Time::Reset();
  lastWindowUpdateTime = 0;
}

void dg::BaseEngine::FixCurrentDirectory() {
  char currentDir[2048] = {};

#if defined(_WIN32)
  GetModuleFileName(0, currentDir, 1024);
#elif defined(__APPLE__)
  uint32_t size = sizeof(currentDir);
  if (_NSGetExecutablePath(currentDir, &size) != 0) {
    throw std::runtime_error("Cannot find executable path");
  }
#endif

  char* lastSlash = strrchr(currentDir, dg::PathSeparator());
  if (lastSlash == nullptr) {
    throw std::runtime_error("Cannot determine executable directory");
  }
  *lastSlash = '\0';

#if defined(_WIN32)
  SetCurrentDirectory(currentDir);
#elif defined(__APPLE__)
  chdir(currentDir);
#endif
}

#pragma region OpenGL
#if defined(_OPENGL)

dg::OpenGLEngine::OpenGLEngine(std::shared_ptr<Window> window)
    : BaseEngine(window) {}

void dg::OpenGLEngine::Initialize() {
  BaseEngine::Initialize();

  // TODO
}

#endif
#pragma endregion
#pragma region DirectX
#if defined(_DIRECTX)

dg::DirectXEngine::DirectXEngine(std::shared_ptr<Window> window)
    : BaseEngine(window) {}

void dg::DirectXEngine::Initialize() {
  InitializeWRL();
  BaseEngine::Initialize();
}

void dg::DirectXEngine::InitializeWRL() {
# if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
  Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(
      RO_INIT_MULTITHREADED);
  if (FAILED(initialize)) {
    throw std::runtime_error("Failed to initialize Windows runtime.");
  }
# else
  HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
  if (FAILED(hr)) {
    throw std::runtime_error("Failed to initialize Windows runtime.");
  }
# endif
}

#endif
#pragma endregion
