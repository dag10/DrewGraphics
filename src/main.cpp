//
//  main.cpp
//

#if defined(_WIN32)
#include <Windows.h>
#endif

#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\client.h>
#include <stdio.h>
#pragma comment(lib, "runtimeobject.lib")

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include "dg/EngineTime.h"
#include "dg/Exceptions.h"
#include "dg/Graphics.h"
#include "dg/InputCodes.h"
#include "dg/Window.h"
#include "dg/scenes/RobotScene.h"

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

static void InitializeWRL() {
# if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
  Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(
      RO_INIT_MULTITHREADED);
  if (FAILED(initialize)) {
    terminateWithError("Failed to initialize Windows runtime.");
  }
# else
  HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
  if (FAILED(hr)) {
    terminateWithError("Failed to initialize Windows runtime.");
  }
# endif
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  Win32Window::CreateConsoleWindow(500, 120, 32, 120);
# if defined(_MSC_VER) & defined(_DEBUG)
  // Enable memory leak detection.
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
# endif

#if defined(_WIN32)
  FixCurrentDirectory();
#endif

  InitializeWRL();

  // Create scene.
  std::string sceneName = "Waving Robot";
  std::unique_ptr<Scene> scene = RobotScene::Make();

  // Create window.
  try {
    window = dg::Win32Window::Open(800, 600, "Drew Graphics", hInstance);
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
        std::string platform = "DirectX";
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
  return 0;
}
