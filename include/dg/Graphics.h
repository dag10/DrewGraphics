//
//  Graphics.h
//

#pragma once

#include <forward_list>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include "dg/FrameBuffer.h"
#include "dg/RasterizerState.h"

#if defined(_OPENGL)
#include "dg/glad/glad.h"

#include <GLFW/glfw3.h>
#elif defined(_DIRECTX)
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#endif

namespace dg {

  class Window;
  class OpenGLGraphics;
  class DirectXGraphics;

  class Graphics {

    public:

#if defined(_OPENGL)
      typedef OpenGLGraphics graphics_class;
#elif defined(_DIRECTX)
      typedef DirectXGraphics graphics_class;
#endif

      static std::unique_ptr<graphics_class> Instance;

      static void Initialize(Window& window);
      static void Shutdown();

      virtual ~Graphics() = default;

      virtual void OnWindowResize(Window& window) {};

      virtual void SetRenderTarget(FrameBuffer &frameBuffer) = 0;
      virtual void SetRenderTarget(Window &window) = 0;
      virtual void SetViewport(int x, int y, int width, int height) = 0;

      virtual void ClearColor(glm::vec3 color, bool clearDepth = true,
                              bool clearStencil = true) = 0;
      virtual void ClearDepthStencil(bool clearDepth = true,
                                     bool clearStencil = true) = 0;

      void PushRasterizerState(const RasterizerState &state);
      void PopRasterizerState();
      void ApplyCurrentRasterizerState();
      const RasterizerState *GetEffectiveRasterizerState() const;

    protected:

      virtual void InitializeGraphics() = 0;
      virtual void InitializeResources();
      virtual void ApplyRasterizerState(const RasterizerState &state) = 0;

      std::forward_list<std::unique_ptr<RasterizerState>> states;

  }; // class Graphics

#if defined(_OPENGL)
  class OpenGLGraphics : public Graphics {
    friend class Graphics;

    public:

      OpenGLGraphics(Window& window);

      virtual void SetRenderTarget(FrameBuffer &frameBuffer);
      virtual void SetRenderTarget(Window &window);
      virtual void SetViewport(int x, int y, int width, int height);

      virtual void ClearColor(glm::vec3 color, bool clearDepth = true,
                              bool clearStencil = true);
      virtual void ClearDepthStencil(bool clearDepth = true,
                                     bool clearStencil = true);

    protected:

      virtual void InitializeGraphics();
      virtual void InitializeResources();
      virtual void ApplyRasterizerState(const RasterizerState &state);

      static GLenum ToGLEnum(RasterizerState::CullMode cullMode);
      static GLenum ToGLEnum(RasterizerState::DepthFunc depthFunc);
      static GLenum ToGLEnum(RasterizerState::BlendEquation blendEquation);
      static GLenum ToGLEnum(RasterizerState::BlendFunc blendFunction);

  }; // class OpenGLGraphics
#endif

#if defined(_DIRECTX)
  class DirectXGraphics : public Graphics {
    friend class Graphics;

    public:

      DirectXGraphics(Window& window);
      virtual ~DirectXGraphics();

      virtual void OnWindowResize(const Window& window);

      virtual void SetRenderTarget(FrameBuffer &frameBuffer);
      virtual void SetRenderTarget(Window &window);
      virtual void SetViewport(int x, int y, int width, int height);

      virtual void ClearColor(glm::vec3 color, bool clearDepth = true,
                              bool clearStencil = true);
      virtual void ClearDepthStencil(bool clearDepth = true,
                                     bool clearStencil = true);

      ID3D11Device *device;
      ID3D11DeviceContext *context;
      D3D_FEATURE_LEVEL dxFeatureLevel;
      IDXGISwapChain *swapChain;

      ID3D11RenderTargetView *windowRenderTargetView;
      ID3D11DepthStencilView *windowDepthStencilView;

      ID3D11RenderTargetView *currentRenderTargetView = NULL;
      ID3D11DepthStencilView *currentDepthStencilView = NULL;

    protected:

      virtual void InitializeGraphics();
      virtual void ApplyRasterizerState(const RasterizerState &state);

    private:

      struct RasterizerStateResources {
        ID3D11RasterizerState *rsState = NULL;
        ID3D11DepthStencilState *dsState = NULL;
        ID3D11BlendState *blendState = NULL;

        ~RasterizerStateResources();
      };

      std::unordered_map<RasterizerState::hash_type,
                         std::shared_ptr<RasterizerStateResources>>
          rasterizerStateResources;

      std::shared_ptr<RasterizerStateResources> CreateRasterizerStateResources(
          const RasterizerState &state);

      Window& window;
      glm::vec2 contentSize;

      static D3D11_CULL_MODE CullModeToDXEnum(
          RasterizerState::CullMode cullMode);
      static D3D11_DEPTH_WRITE_MASK WriteDepthToDXEnum(bool writeDepth);
      static D3D11_COMPARISON_FUNC DepthFuncToDXEnum(
          RasterizerState::DepthFunc depthFunc);
      static D3D11_BLEND_OP BlendEquationToDXEnum(
          RasterizerState::BlendEquation blendEquation);
      static D3D11_BLEND BlendFuncToDXEnum(
          RasterizerState::BlendFunc blendFunction);

  }; // class DirectXGraphics
#endif

} // namespace dg
