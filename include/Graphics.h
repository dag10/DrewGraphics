//
//  Graphics.h
//

#pragma once

#include <memory>

namespace dg {

  class Window;

  class Graphics {

    public:

      static std::unique_ptr<Graphics> Instance;

      static void Initialize(const Window& window);
      static void Shutdown();

      virtual ~Graphics() = default;

    protected:

      void InitializeResources();

  }; // class Graphics

#if defined(_OPENGL)
  class OpenGLGraphics : public Graphics {

    public:

      OpenGLGraphics(const Window& window);
      virtual ~OpenGLGraphics();

  }; // class OpenGLGraphics
#endif

#if defined(_DIRECTX)
  class DirectXGraphics : public Graphics {

    public:

      DirectXGraphics(const Window& window);

  }; // class OpenGLGraphics
#endif

} // namespace dg
