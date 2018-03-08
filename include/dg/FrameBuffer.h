//
//  FrameBuffer.h
//
#pragma once

#include "dg/Texture.h"
#include <memory>
#include <string>
#include "dg/glad/glad.h"

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL resources.
  class RenderBuffer {

    public:
      RenderBuffer(unsigned int width, unsigned int height, GLenum format);
      RenderBuffer(RenderBuffer& other) = delete;
      RenderBuffer(RenderBuffer&& other);
      ~RenderBuffer();
      RenderBuffer& operator=(RenderBuffer& other) = delete;
      RenderBuffer& operator=(RenderBuffer&& other);
      friend void swap(RenderBuffer& first, RenderBuffer& second); // nothrow

      GLuint GetHandle() const;

      unsigned int GetWidth() const;
      unsigned int GetHeight() const;

    private:

      GLuint bufferHandle = 0;
      unsigned int width;
      unsigned int height;

  }; // class RenderBuffer

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL resources.
  class FrameBuffer {

    public:
      FrameBuffer(
        unsigned int width, unsigned int height, bool depthReadable = false,
        bool allowStencil = false, bool createColorTexture = true);
      FrameBuffer(FrameBuffer& other) = delete;
      FrameBuffer(FrameBuffer&& other);
      ~FrameBuffer();
      FrameBuffer& operator=(FrameBuffer& other) = delete;
      FrameBuffer& operator=(FrameBuffer&& other);
      friend void swap(FrameBuffer& first, FrameBuffer& second); // nothrow

      GLuint GetHandle() const;
      void Bind() const;
      static void Unbind();

      unsigned int GetWidth() const;
      unsigned int GetHeight() const;

      std::shared_ptr<Texture> GetColorTexture() const;
      std::shared_ptr<Texture> GetDepthTexture() const;
      std::shared_ptr<RenderBuffer> GetDepthRenderBuffer() const;

      void AttachColorTexture(std::shared_ptr<Texture> texture);
      void AttachDepthTexture(
        std::shared_ptr<Texture> texture, bool allowStencil);
      void AttachDepthRenderBuffer(
        std::shared_ptr<RenderBuffer> buffer, bool allowStencil);

      void SetViewport();

    private:

      GLuint bufferHandle = 0;
      unsigned int width;
      unsigned int height;

      std::shared_ptr<Texture> colorTexture = nullptr;
      std::shared_ptr<Texture> depthTexture = nullptr;
      std::shared_ptr<RenderBuffer> depthRenderBuffer = nullptr;

  }; // class FrameBuffer

} // namespace dg
