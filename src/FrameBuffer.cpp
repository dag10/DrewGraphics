//
//  FrameBuffer.cpp
//

#include <string>
#include <cassert>
#include <memory>

#include <FrameBuffer.h>
#include <Exceptions.h>

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

dg::FrameBuffer::FrameBuffer(
  unsigned int width, unsigned int height, bool depthReadable)
  : width(width), height(height) {
  glGenFramebuffers(1, &bufferHandle);

  AttachColorTexture(std::make_shared<Texture>(
    Texture::WithDimensions(width, height)));

  if (depthReadable) {
    AttachDepthTexture(std::make_shared<Texture>(
      Texture::DepthTexture(width, height)));
  } else {
    AttachDepthRenderBuffer(std::make_shared<RenderBuffer>(
      width, height, GL_DEPTH_COMPONENT));
  }

  GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    throw FrameBufferException(status);
  }
}

dg::FrameBuffer::FrameBuffer(dg::FrameBuffer&& other) {
  *this = std::move(other);
}

dg::FrameBuffer::~FrameBuffer() {
  if (bufferHandle != 0) {
    glDeleteFramebuffers(1, &bufferHandle);
    bufferHandle = 0;
  }
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
  glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);
}

void dg::FrameBuffer::Unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->GetHandle(),
    0);
  Unbind();
}

void dg::FrameBuffer::AttachDepthTexture(std::shared_ptr<Texture> texture) {
  depthTexture = texture;
  Bind();
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->GetHandle(),
    0);
  Unbind();
  depthRenderBuffer = nullptr;
}

void dg::FrameBuffer::AttachDepthRenderBuffer(
  std::shared_ptr<RenderBuffer> buffer) {
  depthRenderBuffer = buffer;
  Bind();
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer->GetHandle());
  Unbind();
  depthTexture = nullptr;
}

#pragma endregion
