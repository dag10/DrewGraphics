//
//  Graphics.h
//

#pragma once

#include <memory>
#include <glm/glm.hpp>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

namespace dg {

  class Window;
  class OpenGLGraphics;
  class DirectXGraphics;

  class Graphics {

    public:

      typedef DirectXGraphics graphics_class;

      static std::unique_ptr<graphics_class> Instance;

      static void Initialize(const Window& window);
      static void Shutdown();

      virtual ~Graphics() = default;

      virtual void OnWindowResize(const Window& window) {};

      virtual void Clear(glm::vec3 color) = 0;

    protected:

      virtual void InitializeGraphics() = 0;
      virtual void InitializeResources();

  }; // class Graphics


  class DirectXGraphics : public Graphics {
    friend class Graphics;

    public:

      DirectXGraphics(const Window& window);
      virtual ~DirectXGraphics();

      virtual void OnWindowResize(const Window& window);

      virtual void Clear(glm::vec3 color);

      ID3D11Device *device;
      ID3D11DeviceContext *context;
      D3D_FEATURE_LEVEL dxFeatureLevel;
      IDXGISwapChain *swapChain;

      ID3D11RenderTargetView *backBufferRTV;
      ID3D11DepthStencilView *depthStencilView;

    protected:

      virtual void InitializeGraphics();

    private:

      const Window& window;
      glm::vec2 contentSize;

  }; // class OpenGLGraphics

} // namespace dg
