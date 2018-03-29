//
//  FrameBuffer.h
//
#pragma once

#include <memory>
#include <string>
#include "dg/Texture.h"
#include "dg/glad/glad.h"

namespace dg {

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

  }; // class RenderBuffer

  // Copy is disabled to prevent resource leaks.
  class FrameBuffer {

    public:

     static std::shared_ptr<FrameBuffer> Create(unsigned int width,
                                                unsigned int height,
                                                bool depthReadable = false,
                                                bool allowStencil = false,
                                                bool createColorTexture = true);

     FrameBuffer(FrameBuffer& other) = delete;
     ~FrameBuffer();
     FrameBuffer& operator=(FrameBuffer& other) = delete;

     GLuint GetHandle() const;
     void Bind() const;
     static void Unbind();

     unsigned int GetWidth() const;
     unsigned int GetHeight() const;

     std::shared_ptr<Texture> GetColorTexture() const;
     std::shared_ptr<Texture> GetDepthTexture() const;
     std::shared_ptr<RenderBuffer> GetDepthRenderBuffer() const;

     void AttachColorTexture(std::shared_ptr<Texture> texture);
     void AttachDepthTexture(std::shared_ptr<Texture> texture,
                             bool allowStencil);
     void AttachDepthRenderBuffer(std::shared_ptr<RenderBuffer> buffer,
                                  bool allowStencil);

     void SetViewport();

    private:

      FrameBuffer(
        unsigned int width, unsigned int height, bool depthReadable,
        bool allowStencil, bool createColorTexture);

      GLuint bufferHandle = 0;
      unsigned int width;
      unsigned int height;

      std::shared_ptr<Texture> colorTexture = nullptr;
      std::shared_ptr<Texture> depthTexture = nullptr;
      std::shared_ptr<RenderBuffer> depthRenderBuffer = nullptr;

  }; // class FrameBuffer

} // namespace dg
