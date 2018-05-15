//
//  main.cpp
//

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include "dg/Engine.h"
#include "dg/Exceptions.h"
#include "dg/Window.h"
#include "dg/scenes/AOScene.h"
#include "dg/scenes/BoundsScene.h"
#include "dg/scenes/CanvasTestScene.h"
#include "dg/scenes/MeshesScene.h"
#include "dg/scenes/QuadScene.h"
#include "dg/scenes/RobotScene.h"
#include "dg/scenes/PointShadowScene.h"
#include "dg/scenes/ShadowScene.h"
#include "dg/scenes/SimpleScene.h"
#include "dg/scenes/TexturesScene.h"
#include "dg/scenes/TransparencyScene.h"
#include "dg/scenes/VRScene.h"
#include "dg/scenes/WidgetScene.h"

using namespace dg;

std::unique_ptr<Scene> PromptForScene(
  const std::string& launchArg) {
  std::map<
    std::string,
    std::function<std::unique_ptr<dg::Scene>()>> constructors;
  constructors["ao"]           = dg::AOScene::Make;
  constructors["shadows"]      = dg::ShadowScene::Make;
  constructors["simple"]       = dg::SimpleScene::Make;
  constructors["widget"]       = dg::WidgetScene::Make;
  constructors["widget-vr"]    = dg::WidgetScene::MakeVR;
  constructors["textures"]     = dg::TexturesScene::Make;
  constructors["meshes"]       = dg::MeshesScene::Make;
  constructors["meshes-vr"]    = dg::MeshesScene::MakeVR;
  constructors["bounds"]       = dg::BoundsScene::Make;
  constructors["robot"]        = dg::RobotScene::Make;
  constructors["robot-vr"]     = dg::RobotScene::MakeVR;
  constructors["pointshadow"]  = dg::PointShadowScene::Make;
  constructors["quad"]         = dg::QuadScene::Make;
  constructors["canvas"]       = dg::CanvasTestScene::Make;
  constructors["vr"]           = dg::VRScene::Make;
  constructors["transparency"] = dg::TransparencyScene::Make;
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

static void RunEngine(std::string sceneName) {
  std::shared_ptr<Scene> scene = PromptForScene(sceneName);
  if (scene == nullptr) {
    return;
  }

  std::shared_ptr<Window> window = nullptr;

  try {
    window = dg::Window::Open(800, 600, "Drew Graphics");
    Engine engine(window);

    engine.Initialize();
    engine.StartScene(scene);

    while (!engine.ShouldQuit()) {
      engine.Update();
    }
  } catch (const std::exception &e) {
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
  RunEngine((argc > 1) ? std::string(argv[1]) : "");
  return 0;
}

#elif defined(_DIRECTX)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {

  Win32Window::SetHInstance(hInstance);
  Win32Window::CreateConsoleWindow(500, 120, 32, 120);

# if defined(_MSC_VER) & defined(_DEBUG)
  // Enable memory leak detection.
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
# endif

  RunEngine(std::string(lpCmdLine));
  return 0;
}

#endif
