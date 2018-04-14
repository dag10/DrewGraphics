//
//  main.cpp
//

#include <iostream>
#include <memory>
#include "cavr/CavrEngine.h"
#include "cavr/scenes/CaveTestScene.h"
#include "cavr/scenes/GameScene.h"
#include "dg/Exceptions.h"
#include "dg/Window.h"

static void RunEngine() {
  std::shared_ptr<dg::Scene> scene = cavr::GameScene::Make();
  std::shared_ptr<dg::Window> window = nullptr;

  try {
    window = dg::Window::Open(800, 600, "Loading CaVR...");
    cavr::CavrEngine engine(window);

    engine.Initialize();
    engine.StartScene(scene);

    std::cout
        << "Press ESC or Q to release the cursor, and press again to quit."
        << std::endl
        << std::endl;

    while (!engine.ShouldQuit()) {
      engine.Update();
    }

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
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
    exit(-1);
  }
}

#if defined(_OPENGL)

int main(int argc, const char* argv[]) {
  RunEngine();
  return 0;
}

#elif defined(_DIRECTX)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {

  dg::Win32Window::SetHInstance(hInstance);
  dg::Win32Window::CreateConsoleWindow(500, 120, 32, 120);

# if defined(_MSC_VER) & defined(_DEBUG)
  // Enable memory leak detection.
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
# endif

  RunEngine();
  return 0;
}

#endif
