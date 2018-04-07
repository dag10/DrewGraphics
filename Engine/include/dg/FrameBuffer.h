//
//  FrameBuffer.h
//
#pragma once

#include <memory>
#include <string>
#include "dg/Texture.h"

#if defined(_OPENGL)
#include "dg/glad/glad.h"
#elif defined(_DIRECTX)
#include <d3d11.h>
#endif

namespace dg {

  class OpenGLFrameBuffer;
  class DirectXFrameBuffer;
#if defined(_OPENGL)
  using FrameBuffer = OpenGLFrameBuffer;
#elif defined(_DIRECTX)
  using FrameBuffer = DirectXFrameBuffer;
#endif

#if defined(_OPENGL)
  // Copy is disabled to prevent resource leaks.
  class RenderBuffer {

   public:

    static std::shared_ptr<RenderBuffer> Create(unsigned int width,
                                                unsigned int height,
                                                GLenum format);

    RenderBuffer(RenderBuffer& other) = delete;
    ~RenderBuffer();
    RenderBuffer& operator=(RenderBuffer& other) = delete;

    GLuint GetHandle() const;

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

   private:

    RenderBuffer(unsigned int width, unsigned int height, GLenum format);

    GLuint bufferHandle = 0;
    unsigned int width;
    unsigned int height;

  };  // class RenderBuffer
#endif

  // Copy is disabled to prevent resource leaks.
  class BaseFrameBuffer {

   public:

    static std::shared_ptr<FrameBuffer> Create(
        unsigned int width, unsigned int height, bool depthReadable = false,
        bool allowStencil = false, bool createColorTexture = true);

    BaseFrameBuffer(BaseFrameBuffer& other) = delete;
    virtual ~BaseFrameBuffer() {}
    BaseFrameBuffer& operator=(BaseFrameBuffer& other) = delete;

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

    std::shared_ptr<Texture> GetColorTexture() const;
    std::shared_ptr<Texture> GetDepthTexture() const;

   protected:

    BaseFrameBuffer() {}

    unsigned int width = 0;
    unsigned int height = 0;

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

    OpenGLFrameBuffer(unsigned int width, unsigned int height,
                      bool depthReadable, bool allowStencil,
                      bool createColorTexture);


    void AttachColorTexture(std::shared_ptr<Texture> texture);
    void AttachDepthTexture(std::shared_ptr<Texture> texture,
                            bool allowStencil);
    void AttachDepthRenderBuffer(std::shared_ptr<RenderBuffer> buffer,
                                 bool allowStencil);

    GLuint bufferHandle = 0;
    std::shared_ptr<RenderBuffer> depthRenderBuffer = nullptr;

  }; // class OpenGLFrameBuffer

#elif defined(_DIRECTX)

  class DirectXFrameBuffer : public BaseFrameBuffer {
    friend class BaseFrameBuffer;

   public:

    virtual ~DirectXFrameBuffer();

    DirectXFrameBuffer(unsigned int width, unsigned int height,
                       bool depthReadable, bool allowStencil,
                       bool createColorTexture);

    ID3D11RenderTargetView *GetRenderTargetView();
    ID3D11DepthStencilView *GetDepthStencilView();

   private:

    ID3D11RenderTargetView *renderTargetView = NULL;
    ID3D11DepthStencilView *depthStencilView = NULL;

  }; // class DirectXFrameBuffer

#endif

} // namespace dg
