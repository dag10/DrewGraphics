//
//  FrameBuffer.cpp
//

#include "dg/FrameBuffer.h"
#include <cassert>
#include <memory>
#include <string>
#include "dg/Exceptions.h"
#include "dg/Graphics.h"

#pragma region RenderBuffer
#if defined(_OPENGL)

std::shared_ptr<dg::RenderBuffer> dg::RenderBuffer::Create(unsigned int width,
                                                           unsigned int height,
                                                           GLenum format) {
  return std::shared_ptr<RenderBuffer>(new RenderBuffer(width, height, format));
}

dg::RenderBuffer::RenderBuffer(
  unsigned int width, unsigned int height, GLenum format) {
  glGenRenderbuffers(1, &bufferHandle);
  glBindRenderbuffer(GL_RENDERBUFFER, bufferHandle);
  glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

dg::RenderBuffer::~RenderBuffer() {
  if (bufferHandle != 0) {
    glDeleteRenderbuffers(1, &bufferHandle);
    bufferHandle = 0;
  }
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

#endif
#pragma endregion

#pragma region BaseFrameBuffer

std::shared_ptr<dg::FrameBuffer> dg::BaseFrameBuffer::Create(
    unsigned int width, unsigned int height, bool depthReadable,
    bool allowStencil, bool createColorTexture) {
  return std::shared_ptr<FrameBuffer>(new FrameBuffer(
      width, height, depthReadable, allowStencil, createColorTexture));
}

unsigned int dg::BaseFrameBuffer::GetWidth() const {
  return width;
}

unsigned int dg::BaseFrameBuffer::GetHeight() const {
  return height;
}

std::shared_ptr<dg::Texture> dg::BaseFrameBuffer::GetColorTexture() const {
  return colorTexture;
}

std::shared_ptr<dg::Texture> dg::BaseFrameBuffer::GetDepthTexture() const {
  return depthTexture;
}

#pragma endregion

#if defined(_OPENGL)
#pragma region OpenGL FrameBuffer

dg::OpenGLFrameBuffer::OpenGLFrameBuffer(unsigned int width,
                                         unsigned int height,
                                         bool depthReadable, bool allowStencil,
                                         bool createColorTexture) {
  this->width = width;
  this->height = height;

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
      AttachDepthTexture(Texture::DepthTexture(width, height, true, true),
                         true);
    } else {
      AttachDepthTexture(Texture::DepthTexture(width, height, false, true),
                         false);
    }
  } else {
    if (allowStencil) {
      AttachDepthRenderBuffer(
          RenderBuffer::Create(width, height, GL_DEPTH_STENCIL), true);
    } else {
      AttachDepthRenderBuffer(
          RenderBuffer::Create(width, height, GL_DEPTH_COMPONENT), false);
    }
  }

  GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    throw FrameBufferException(status);
  }
}

dg::OpenGLFrameBuffer::~OpenGLFrameBuffer() {
  if (bufferHandle != 0) {
    glDeleteFramebuffers(1, &bufferHandle);
    bufferHandle = 0;
  }
}

void dg::OpenGLFrameBuffer::AttachColorTexture(std::shared_ptr<Texture> texture) {
  colorTexture = texture;
  glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->GetHandle(),
    0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void dg::OpenGLFrameBuffer::AttachDepthTexture(
  std::shared_ptr<Texture> texture, bool allowStencil) {
  depthTexture = texture;
  glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);
  GLenum format = allowStencil \
    ? GL_DEPTH_STENCIL_ATTACHMENT
    : GL_DEPTH_ATTACHMENT;
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, format, GL_TEXTURE_2D, texture->GetHandle(),
    0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  depthRenderBuffer = nullptr;
}

void dg::OpenGLFrameBuffer::AttachDepthRenderBuffer(
  std::shared_ptr<RenderBuffer> buffer, bool allowStencil) {
  depthRenderBuffer = buffer;
  glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);
  GLenum format = allowStencil \
    ? GL_DEPTH_STENCIL_ATTACHMENT
    : GL_DEPTH_ATTACHMENT;
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, format, GL_RENDERBUFFER, buffer->GetHandle());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  depthTexture = nullptr;
}


GLuint dg::OpenGLFrameBuffer::GetHandle() const {
  return bufferHandle;
}

#pragma endregion
#elif defined(_DIRECTX)
#pragma region DirectX FrameBuffer

dg::DirectXFrameBuffer::DirectXFrameBuffer(unsigned int width,
                                           unsigned int height,
                                           bool depthReadable,
                                           bool allowStencil,
                                           bool createColorTexture) {
  this->width = width;
  this->height = height;

  depthTexture =
      Texture::DepthTexture(width, height, allowStencil, depthReadable);

  if (createColorTexture) {
    TextureOptions texOpts;
    texOpts.width = width;
    texOpts.height = height;
    texOpts.wrap = TextureWrap::CLAMP_EDGE;
    colorTexture = Texture::Generate(texOpts);

    Graphics::Instance->device->CreateRenderTargetView(
        colorTexture->GetTexture(), 0, &renderTargetView);
  }

  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Texture2D.MipSlice = 0;
  depthStencilViewDesc.Format =
      depthTexture->GetOptions().GetDirectXDepthStencilFormat();

  ID3D11Texture2D *depthTex = depthTexture->GetTexture();
  HRESULT hr = Graphics::Instance->device->CreateDepthStencilView(
      depthTex, &depthStencilViewDesc, &depthStencilView);

  if (FAILED(hr)) {
    throw EngineError("Failed to create FrameBuffer's DepthStencilView.");
  }
}

dg::DirectXFrameBuffer::~DirectXFrameBuffer() {
  if (renderTargetView != NULL) {
    renderTargetView->Release();
    renderTargetView = NULL;
  }
  if (depthStencilView != NULL) {
    depthStencilView->Release();
    depthStencilView = NULL;
  }
}

ID3D11RenderTargetView *dg::DirectXFrameBuffer::GetRenderTargetView() {
  return renderTargetView;
}

ID3D11DepthStencilView *dg::DirectXFrameBuffer::GetDepthStencilView() {
  return depthStencilView;
}

#pragma endregion
#endif
