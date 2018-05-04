//
//  FrameBuffer.h
//
#pragma once

#include <memory>
#include <string>
#include "dg/Texture.h"

#if defined(_OPENGL)
#include "dg/opengl/glad/glad.h"
#elif defined(_DIRECTX)
#include <d3d11.h>
#endif

namespace dg {

#pragma region FrameBuffer Options

#pragma endregion

  class OpenGLFrameBuffer;
  class DirectXFrameBuffer;
#if defined(_OPENGL)
  using FrameBuffer = OpenGLFrameBuffer;
#elif defined(_DIRECTX)
  using FrameBuffer = DirectXFrameBuffer;
#endif

  // Copy is disabled to prevent resource leaks.
  class BaseFrameBuffer {

   public:

    struct Options {
      unsigned int width;
      unsigned int height;
      bool depthReadable = true;
      bool hasColor = true;
      bool hasStencil = true;
      bool mipmap = false;
      TextureInterpolation interpolation = TextureInterpolation::LINEAR;
    };

    static std::shared_ptr<FrameBuffer> Create(Options options);

    BaseFrameBuffer(BaseFrameBuffer& other) = delete;
    virtual ~BaseFrameBuffer() {}
    BaseFrameBuffer& operator=(BaseFrameBuffer& other) = delete;

    const Options &GetOptions() const;
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    float GetAspectRatio() const;

    std::shared_ptr<Texture> GetColorTexture() const;
    std::shared_ptr<Texture> GetDepthTexture() const;

   protected:

    BaseFrameBuffer(Options options);

    const Options options;

    std::shared_ptr<Texture> colorTexture = nullptr;
    std::shared_ptr<Texture> depthTexture = nullptr;

  }; // class BaseFrameBuffer

#if defined(_OPENGL)

  class OpenGLFrameBuffer : public BaseFrameBuffer {
    friend class BaseFrameBuffer;

   public:

    virtual ~OpenGLFrameBuffer();

    GLuint GetHandle() const;

   private:

    OpenGLFrameBuffer(Options options);

    GLuint bufferHandle = 0;

  }; // class OpenGLFrameBuffer

#elif defined(_DIRECTX)

  class DirectXFrameBuffer : public BaseFrameBuffer {
    friend class BaseFrameBuffer;

   public:

    virtual ~DirectXFrameBuffer();

    DirectXFrameBuffer(Options options);

    ID3D11RenderTargetView *GetRenderTargetView();
    ID3D11DepthStencilView *GetDepthStencilView();

   private:

    ID3D11RenderTargetView *renderTargetView = NULL;
    ID3D11DepthStencilView *depthStencilView = NULL;

  }; // class DirectXFrameBuffer

#endif

} // namespace dg
