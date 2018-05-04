//
//  FrameBuffer.cpp
//

#include "dg/FrameBuffer.h"
#include <cassert>
#include <memory>
#include <string>
#include "dg/Exceptions.h"
#include "dg/Graphics.h"

#pragma region BaseFrameBuffer

std::shared_ptr<dg::FrameBuffer> dg::BaseFrameBuffer::Create(Options options) {
  return std::shared_ptr<FrameBuffer>(new FrameBuffer(options));
}

dg::BaseFrameBuffer::BaseFrameBuffer(Options options) : options(options) {
  // Create depth (and possibly stencil) texture.
  TextureOptions texOpts;
  texOpts.width = options.width;
  texOpts.height = options.height;
  texOpts.format = options.hasStencil ? TexturePixelFormat::DEPTH_STENCIL
                                      : TexturePixelFormat::DEPTH;
  texOpts.type =
      options.hasStencil ? TexturePixelType::INT : TexturePixelType::FLOAT;
  texOpts.wrap = TextureWrap::CLAMP_EDGE;
  texOpts.mipmap = false;
  texOpts.shaderReadable = options.depthReadable;
  texOpts.mipmap = options.mipmap;
  depthTexture = Texture::Generate(texOpts);

  // Create main texture.
  if (options.hasColor) {
    TextureOptions texOpts;
    texOpts.width = options.width;
    texOpts.height = options.height;
    texOpts.wrap = TextureWrap::CLAMP_EDGE;
    texOpts.mipmap = options.mipmap;
    texOpts.interpolation = options.interpolation;
    colorTexture = Texture::Generate(texOpts);
  }
}

const dg::FrameBuffer::Options &dg::BaseFrameBuffer::GetOptions() const {
  return options;
}

unsigned int dg::BaseFrameBuffer::GetWidth() const {
  return options.width;
}

unsigned int dg::BaseFrameBuffer::GetHeight() const {
  return options.height;
}

float dg::BaseFrameBuffer::GetAspectRatio() const {
  return (float)GetWidth() / (float)GetHeight();
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

dg::OpenGLFrameBuffer::OpenGLFrameBuffer(Options options)
    : BaseFrameBuffer(options) {
  glGenFramebuffers(1, &bufferHandle);

  if (options.hasColor) {
    glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           colorTexture->GetHandle(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);
  GLenum format =
      options.hasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
  glFramebufferTexture2D(GL_FRAMEBUFFER, format, GL_TEXTURE_2D,
                         depthTexture->GetHandle(), 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

GLuint dg::OpenGLFrameBuffer::GetHandle() const {
  return bufferHandle;
}

#pragma endregion
#elif defined(_DIRECTX)
#pragma region DirectX FrameBuffer

dg::DirectXFrameBuffer::DirectXFrameBuffer(Options options)
    : BaseFrameBuffer(options) {
  if (options.hasColor) {
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
