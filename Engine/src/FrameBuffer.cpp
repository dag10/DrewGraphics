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
  // Ensure all textures are the same type.
  TextureType type = options.textureOptions.empty()
                         ? TextureType::_2D
                         : options.textureOptions[0].type;
  for (auto &texOpts : options.textureOptions) {
    if (texOpts.type != type) {
      throw EngineError(
          "Framebuffer::Options::textureOptions have inconsistent "
          "TextureTypes.");
    }
  }

  // Create depth (and possibly stencil) texture.
  TextureOptions depthTexOpts;
  depthTexOpts.type = type;
  depthTexOpts.width = options.width;
  depthTexOpts.height = options.height;
  depthTexOpts.format = options.hasStencil ? TexturePixelFormat::DEPTH_STENCIL
                                      : TexturePixelFormat::DEPTH;
  depthTexOpts.pixelType =
      options.hasStencil ? TexturePixelType::INT : TexturePixelType::FLOAT;
  depthTexOpts.wrap = TextureWrap::CLAMP_EDGE;
  depthTexOpts.shaderReadable = options.depthReadable;
  depthTexture = Texture::Generate(depthTexOpts);

  // If the options want us to have a color texture but didn't provide at least
  // one color texture definition, create a standard one.
  if (options.hasColor && options.textureOptions.empty()) {
    TextureOptions texOpts;
    texOpts.width = options.width;
    texOpts.height = options.height;
    texOpts.wrap = TextureWrap::CLAMP_EDGE;
    options.textureOptions.push_back(texOpts);
  }
  options.hasColor = !options.textureOptions.empty();

  // Create color texture(s).
  for (const auto &texOpts : options.textureOptions) {
    if (texOpts.width != options.width || texOpts.height != options.height) {
      throw EngineError(
          "Attempted to create a framebuffer color texture with dimensions "
          "that do not match those of the framebuffer.");
    }
    colorTextures.push_back(Texture::Generate(texOpts));
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

unsigned int dg::BaseFrameBuffer::ColorTextureCount() const {
  return colorTextures.size();
}

std::shared_ptr<dg::Texture> dg::BaseFrameBuffer::GetColorTexture(int i) const {
  return colorTextures[i];
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
  glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);

  for (int i = 0; i < colorTextures.size(); i++) {
    auto &tex = colorTextures[i];
    switch (tex->GetOptions().type) {
      case TextureType::_2D:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                               GL_TEXTURE_2D, tex->GetHandle(), 0);
        break;
      case TextureType::CUBEMAP:
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                             tex->GetHandle(), 0);
        break;
    }
  }

  GLenum format =
      options.hasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
  switch (depthTexture->GetOptions().type) {
    case TextureType::_2D:
      glFramebufferTexture2D(GL_FRAMEBUFFER, format, GL_TEXTURE_2D,
                             depthTexture->GetHandle(), 0);
    case TextureType::CUBEMAP:
      glFramebufferTexture(GL_FRAMEBUFFER, format, depthTexture->GetHandle(),
                           0);
      break;
  }

  //glDrawBuffer(GL_NONE);
  //glReadBuffer(GL_NONE);

  GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    throw FrameBufferException(status);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
  if (options.textureOptions.size() > 1) {
    throw EngineError(
        "TODO: Multiple color textures not yet implemented for DirectX build.");
  }

  if (options.hasColor) {
    Graphics::Instance->device->CreateRenderTargetView(
        GetColorTexture()->GetTexture(), 0, &renderTargetView);
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
