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
                             bool depthReadable, bool allowStencil)
    : width(width), height(height) {

  // TODO
  std::cerr << "WARNING: FrameBuffer support not yet implemented for DirectX."
            << std::endl;
}

dg::FrameBuffer::FrameBuffer(dg::FrameBuffer&& other) {
  *this = std::move(other);
}

dg::FrameBuffer::~FrameBuffer() {
  // TODO
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
  // TODO
}

void dg::FrameBuffer::Unbind() {
  // TODO
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
  // TODO
  Unbind();
}

void dg::FrameBuffer::AttachDepthTexture(
  std::shared_ptr<Texture> texture, bool allowStencil) {
  depthTexture = texture;
  Bind();
  // TODO
  Unbind();
  depthRenderBuffer = nullptr;
}

void dg::FrameBuffer::AttachDepthRenderBuffer(
  std::shared_ptr<RenderBuffer> buffer, bool allowStencil) {
  depthRenderBuffer = buffer;
  Bind();
  // TODO
  Unbind();
  depthTexture = nullptr;
}

void dg::FrameBuffer::SetViewport() {
  // TODO
}

#pragma endregion
