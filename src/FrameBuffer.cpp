//
//  FrameBuffer.cpp
//

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "dg/Exceptions.h"
#include "dg/FrameBuffer.h"

#pragma region RenderBuffer

dg::RenderBuffer::RenderBuffer(
  unsigned int width, unsigned int height, GLenum format) {
  glGenRenderbuffers(1, &bufferHandle);
  glBindRenderbuffer(GL_RENDERBUFFER, bufferHandle);
  glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

dg::RenderBuffer::RenderBuffer(dg::RenderBuffer&& other) {
  *this = std::move(other);
}

dg::RenderBuffer::~RenderBuffer() {
  if (bufferHandle != 0) {
    glDeleteRenderbuffers(1, &bufferHandle);
    bufferHandle = 0;
  }
}

dg::RenderBuffer& dg::RenderBuffer::operator=(dg::RenderBuffer&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(RenderBuffer& first, RenderBuffer& second) {
  using std::swap;
  swap(first.bufferHandle, second.bufferHandle);
  swap(first.width, second.width);
  swap(first.height, second.height);
}

GLuint dg::RenderBuffer::GetHandle() const {
  return bufferHandle;
}

unsigned int dg::RenderBuffer::GetWidth() const {
  return width;
}

unsigned int dg::RenderBuffer::GetHeight() const {
  return height;
}

#pragma endregion

#pragma region FrameBuffer

dg::FrameBuffer::FrameBuffer(unsigned int width, unsigned int height,
                             bool depthReadable, bool allowStencil,
                             bool createColorTexture)
    : width(width), height(height) {
#if defined(_OPENGL)
  glGenFramebuffers(1, &bufferHandle);

  if (createColorTexture) {
    TextureOptions texOpts;
    texOpts.width = width;
    texOpts.height = height;
    texOpts.wrap = TextureWrap::CLAMP_EDGE;
    AttachColorTexture(Texture::Generate(texOpts));
  }

  if (depthReadable) {
    if (allowStencil) {
      AttachDepthTexture(Texture::DepthTexture(width, height, true), true);
    } else {
      AttachDepthTexture(Texture::DepthTexture(width, height, false), false);
    }
  } else {
    if (allowStencil) {
      AttachDepthRenderBuffer(std::make_shared<RenderBuffer>(
        width, height, GL_DEPTH_STENCIL), true);
    } else {
      AttachDepthRenderBuffer(std::make_shared<RenderBuffer>(
        width, height, GL_DEPTH_COMPONENT), false);
    }
  }

  GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    throw FrameBufferException(status);
  }
#elif defined(_DIRECTX)
  // TODO
  std::cerr << "WARNING: FrameBuffer support not yet implemented for DirectX."
            << std::endl;
#endif
}

dg::FrameBuffer::FrameBuffer(dg::FrameBuffer&& other) {
  *this = std::move(other);
}

dg::FrameBuffer::~FrameBuffer() {
#if defined(_OPENGL)
  if (bufferHandle != 0) {
    glDeleteFramebuffers(1, &bufferHandle);
    bufferHandle = 0;
  }
#elif defined(_DIRECTX)
  // TODO
#endif
}

dg::FrameBuffer& dg::FrameBuffer::operator=(dg::FrameBuffer&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(FrameBuffer& first, FrameBuffer& second) {
  using std::swap;
  swap(first.bufferHandle, second.bufferHandle);
  swap(first.colorTexture, second.colorTexture);
  swap(first.depthTexture, second.depthTexture);
  swap(first.depthRenderBuffer, second.depthRenderBuffer);
  swap(first.width, second.width);
  swap(first.height, second.height);
}

GLuint dg::FrameBuffer::GetHandle() const {
  return bufferHandle;
}

void dg::FrameBuffer::Bind() const {
#if defined(_OPENGL)
  glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);
#elif defined(_DIRECTX)
  // TODO
#endif
}

void dg::FrameBuffer::Unbind() {
#if defined(_OPENGL)
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
#elif defined(_DIRECTX)
  // TODO
#endif
}

unsigned int dg::FrameBuffer::GetWidth() const {
  return width;
}

unsigned int dg::FrameBuffer::GetHeight() const {
  return height;
}

std::shared_ptr<dg::Texture> dg::FrameBuffer::GetColorTexture() const {
  return colorTexture;
}

std::shared_ptr<dg::Texture> dg::FrameBuffer::GetDepthTexture() const {
  return depthTexture;
}

std::shared_ptr<dg::RenderBuffer>
dg::FrameBuffer::GetDepthRenderBuffer() const {
  return depthRenderBuffer;
}

void dg::FrameBuffer::AttachColorTexture(std::shared_ptr<Texture> texture) {
  colorTexture = texture;
  Bind();
#if defined(_OPENGL)
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->GetHandle(),
    0);
#elif defined(_DIRECTX)
  // TODO
#endif
  Unbind();
}

void dg::FrameBuffer::AttachDepthTexture(
  std::shared_ptr<Texture> texture, bool allowStencil) {
  depthTexture = texture;
  Bind();
#if defined(_OPENGL)
  GLenum format = allowStencil \
    ? GL_DEPTH_STENCIL_ATTACHMENT
    : GL_DEPTH_ATTACHMENT;
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, format, GL_TEXTURE_2D, texture->GetHandle(),
    0);
#elif defined(_DIRECTX)
  // TODO
#endif
  Unbind();
  depthRenderBuffer = nullptr;
}

void dg::FrameBuffer::AttachDepthRenderBuffer(
  std::shared_ptr<RenderBuffer> buffer, bool allowStencil) {
  depthRenderBuffer = buffer;
  Bind();
#if defined(_OPENGL)
  GLenum format = allowStencil \
    ? GL_DEPTH_STENCIL_ATTACHMENT
    : GL_DEPTH_ATTACHMENT;
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, format, GL_RENDERBUFFER, buffer->GetHandle());
#elif defined(_DIRECTX)
  // TODO
#endif
  Unbind();
  depthTexture = nullptr;
}

void dg::FrameBuffer::SetViewport() {
#if defined(_OPENGL)
  glViewport(0, 0, width, height);
#elif defined(_DIRECTX)
  // TODO
#endif
}

#pragma endregion
