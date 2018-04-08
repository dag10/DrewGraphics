//
//  Engine.h
//

#pragma once

#if !defined(_OPENGL) & defined(_OPENGL)
#error "No graphics platform specified. Define either _OPENGL or _DIRECTX."
#endif

#if defined(_OPENGL)
#include "dg/opengl/glad/glad.h"

#include <GLFW/glfw3.h>
#endif

#if defined(_WIN32)
#include <Windows.h>
#endif

#if defined(_DIRECTX)
#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#pragma comment(lib, "runtimeobject.lib")
#endif

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

namespace dg {

#if defined(_OPENGL)
  class OpenGLEngine;
  using Engine = OpenGLEngine;
#elif defined(_DIRECTX)
  class DirectXEngine;
  using Engine = DirectXEngine;
#endif

  class Window;
  class Scene;

  class BaseEngine {

    public:

      static inline Engine &Instance() {
        return (Engine&)*instance;
      }

      BaseEngine(BaseEngine &) = delete;
      BaseEngine &operator=(BaseEngine &) = delete;

      virtual ~BaseEngine();

      inline std::shared_ptr<Window> GetWindow() {
        return window;
      }

      inline std::shared_ptr<Scene> CurrentScene() {
        return scene;
      }

      virtual void Initialize();
      virtual void StartScene(std::shared_ptr<Scene> scene);
      virtual void Update();
      virtual bool ShouldQuit() const;

    protected:

      BaseEngine(std::shared_ptr<Window> window);

      static BaseEngine *instance;

      const std::shared_ptr<Window> window = nullptr;
      std::shared_ptr<Scene> scene = nullptr;
      std::shared_ptr<Scene> nextScene = nullptr;
      bool cursorWasLocked = false;
      double lastWindowUpdateTime = 0;

      void StartNextScene();
      void FixCurrentDirectory();

      virtual void UpdateWindowTitle();

  }; // class Engine

#if defined(_OPENGL)

  class OpenGLEngine : public BaseEngine {

    public:

      OpenGLEngine(std::shared_ptr<Window> window);

      virtual void Initialize();

  }; // class OpenGLEngine

#endif

#if defined(_DIRECTX)

  class DirectXEngine : public BaseEngine {

    public:

      DirectXEngine(std::shared_ptr<Window> window);

      virtual void Initialize();

    private:

      void InitializeWRL();

  }; // class DirectXEngine

#endif

} // namespace dg
