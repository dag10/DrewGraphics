//
//  Texture.cpp
//

#if defined(_DIRECTX)
#include <DDSTextureLoader.h>
#endif

#include <cassert>
#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "dg/stb_image.h"

#include "dg/Exceptions.h"
#include "dg/Graphics.h"
#include "dg/Texture.h"

#pragma region Texture Base Class

std::shared_ptr<dg::Texture> dg::BaseTexture::FromPath(const std::string& path) {
  int nrChannels;
  int width;
  int height;

  stbi_set_flip_vertically_on_load(true);

  std::unique_ptr<stbi_uc[]> pixels = std::unique_ptr<stbi_uc[]>(stbi_load(
    path.c_str(), &width, &height, &nrChannels, 4));

  if (pixels == nullptr) {
    throw dg::STBLoadError(path, stbi_failure_reason());
  }

  TextureOptions texOpts;
  texOpts.width = (unsigned int)width;
  texOpts.height = (unsigned int)height;
  texOpts.format = TexturePixelFormat::RGBA;
  texOpts.type = TexturePixelType::BYTE;
  texOpts.mipmap = true;

  auto texture = std::shared_ptr<Texture>(new Texture(texOpts));
  texture->GenerateImage(pixels.get());
  return texture;
}

std::shared_ptr<dg::Texture> dg::BaseTexture::Generate(TextureOptions options) {
  auto texture = std::shared_ptr<Texture>(new Texture(options));
  texture->GenerateImage(nullptr);
  return texture;
}

std::shared_ptr<dg::Texture> dg::BaseTexture::DepthTexture(
    unsigned int width, unsigned int height, bool allowStencil,
    bool shaderReadable) {
  TextureOptions texOpts;
  texOpts.width = width;
  texOpts.height = height;
  texOpts.format = allowStencil ? TexturePixelFormat::DEPTH_STENCIL
                                : TexturePixelFormat::DEPTH;
  texOpts.type = allowStencil ? TexturePixelType::INT : TexturePixelType::FLOAT;
  texOpts.wrap = TextureWrap::CLAMP_EDGE;
  texOpts.mipmap = false;
  texOpts.shaderReadable = shaderReadable;

  return Generate(texOpts);
}

dg::BaseTexture::BaseTexture(TextureOptions options) : options(options) {
  if (options.format == TexturePixelFormat::DEPTH ||
      options.format == TexturePixelFormat::DEPTH_STENCIL) {
    if (options.type == TexturePixelType::BYTE) {
      throw EngineError(
          "Cannot create a depth[+stencil] Texture with byte type.");
    }
    if (options.mipmap) {
      throw EngineError(
          "Cannot create a depth[+stencil] Texture with mipmap enabled.");
    }
  }
  if (options.format == TexturePixelFormat::DEPTH) {
    if (options.type == TexturePixelType::INT) {
      throw EngineError(
          "Cannot create a depth-only Texture with int type. Must use float.");
    }
  }
}

const dg::TextureOptions dg::BaseTexture::GetOptions() const {
  return options;
}

unsigned int dg::BaseTexture::GetWidth() const {
  return options.width;
}

unsigned int dg::BaseTexture::GetHeight() const {
  return options.height;
}

#pragma endregion

#pragma region OpenGL Texture
#if defined(_OPENGL)

dg::OpenGLTexture::OpenGLTexture(TextureOptions options)
    : BaseTexture(options) {}

GLuint dg::OpenGLTexture::GetHandle() const { return textureHandle; }

dg::OpenGLTexture::~OpenGLTexture() {
  if (textureHandle != 0) {
    glDeleteTextures(1, &textureHandle);
    textureHandle = 0;
  }
}

void dg::OpenGLTexture::UpdateData(const void *pixels, bool genMipMap) {
  glBindTexture(GL_TEXTURE_2D, textureHandle);
  glTexSubImage2D(
      GL_TEXTURE_2D,
      0,
      0,
      0,
      GetWidth(),
      GetHeight(),
      options.GetOpenGLInternalFormat(),
      options.GetOpenGLType(),
      pixels);
  if (options.mipmap && genMipMap) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void dg::OpenGLTexture::GenerateImage(void *pixels) {
  assert(textureHandle == 0);

  glGenTextures(1, &textureHandle);

  glBindTexture(GL_TEXTURE_2D, textureHandle);

  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options.GetOpenGLMinFilter());
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options.GetOpenGLMagFilter());
  GLenum wrap = options.GetOpenGLWrap();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

  glTexImage2D(
      GL_TEXTURE_2D,
      0, // Level of detail
      options.GetOpenGLInternalFormat(), // Internal format
      options.width,
      options.height,
      0, // Border
      options.GetOpenGLExternalFormat(), // External format
      options.GetOpenGLType(), // Type
      pixels
      );

  if (pixels != nullptr && options.mipmap) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

#endif
#pragma endregion
#pragma region DirectX Texture
#if defined(_DIRECTX)

dg::DirectXTexture::DirectXTexture(TextureOptions options)
    : BaseTexture(options) {}

dg::DirectXTexture::~DirectXTexture() {
  if (texture != nullptr) {
    texture->Release();
    texture = nullptr;
  }
  if (srv != nullptr) {
    srv->Release();
    srv = nullptr;
  }
  if (sampler != nullptr) {
    sampler->Release();
    sampler = nullptr;
  }
}

void dg::DirectXTexture::UpdateData(const void *pixels, bool genMipMap) {
  size_t rowPitch = (options.width * options.GetDirectXBitsPerPixel() + 7) / 8;
  Graphics::Instance->context->UpdateSubresource(texture, 0, NULL, pixels,
                                                 (unsigned int)rowPitch, 0);
  if (options.mipmap && genMipMap) {
    Graphics::Instance->context->GenerateMips(srv);
  }
}

ID3D11ShaderResourceView *dg::DirectXTexture::GetShaderResourceView() const {
  return srv;
}

ID3D11SamplerState *dg::DirectXTexture::GetSamplerState() const {
  return sampler;
}

ID3D11Texture2D *dg::DirectXTexture::GetTexture() const {
  return texture;
}

void dg::DirectXTexture::GenerateImage(void *pixels) {
  assert(texture == nullptr);

  auto internalFormat = options.GetDirectXInternalFormat();

  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = options.width;
  desc.Height = options.height;
  desc.MipLevels = options.mipmap ? 0 : 1;
  desc.ArraySize = 1;
  desc.Format = internalFormat;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = options.GetDirectXBind();
  if (options.shaderReadable) {
    desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
  }
  if (options.mipmap) {
    desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
  }

  HRESULT hr =
      Graphics::Instance->device->CreateTexture2D(&desc, nullptr, &texture);

  if (FAILED(hr)) {
    throw EngineError("Failed to create texture.");
  }

  if (pixels != nullptr) {
    UpdateData(pixels, false);
  }

  if (options.shaderReadable) {
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = options.GetDirectXShaderFormat();
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = -1;

    hr = Graphics::Instance->device->CreateShaderResourceView(texture, &SRVDesc,
      &srv);

    if (FAILED(hr)) {
      throw EngineError("Failed to create shader resource view.");
    }
  }

  D3D11_SAMPLER_DESC samplerDesc = {};
  samplerDesc.AddressU = options.GetDirectXAddressMode();
  samplerDesc.AddressV = options.GetDirectXAddressMode();
  samplerDesc.AddressW = options.GetDirectXAddressMode();
  samplerDesc.Filter = options.GetDirectXFilter();
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
  hr = Graphics::Instance->device->CreateSamplerState(&samplerDesc, &sampler);

  if (FAILED(hr)) {
    throw EngineError("Failed to create sampler.");
  }

  if (options.mipmap && srv != NULL) {
    Graphics::Instance->context->GenerateMips(srv);
  }
}

#endif
#pragma endregion

#pragma region TextureOptions

#if defined(_OPENGL)

GLenum dg::TextureOptions::GetOpenGLWrap() const {
  switch (wrap) {
    case TextureWrap::REPEAT:
      return GL_REPEAT;
    case TextureWrap::CLAMP_EDGE:
      return GL_CLAMP_TO_EDGE;
    case TextureWrap::CLAMP_BORDER:
      return GL_CLAMP_TO_BORDER;
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLMinFilter() const {
  // Minification filter. Will be the same as the magnification filter,
  // but if mipmap is requested, will also linearly interpolate between
  // the two closest mip levels.
  if (mipmap) {
    switch (interpolation) {
      case TextureInterpolation::NEAREST:
        return GL_NEAREST_MIPMAP_LINEAR;
      case TextureInterpolation::LINEAR:
        return GL_LINEAR_MIPMAP_LINEAR;
    }
  } else {
    return GetOpenGLMagFilter();
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLMagFilter() const {
  switch (interpolation) {
    case TextureInterpolation::NEAREST:
      return GL_NEAREST;
    case TextureInterpolation::LINEAR:
      return GL_LINEAR;
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLInternalFormat() const {
  switch (format) {
    case TexturePixelFormat::RGBA:
      return GL_RGBA;
    case TexturePixelFormat::DEPTH:
      return GL_DEPTH_COMPONENT;
    case TexturePixelFormat::DEPTH_STENCIL:
      return GL_DEPTH24_STENCIL8;
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLExternalFormat() const {
  switch (format) {
    case TexturePixelFormat::DEPTH_STENCIL:
      return GL_DEPTH_STENCIL;
    default:
      return GetOpenGLInternalFormat();
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLType() const {
  if (format == TexturePixelFormat::DEPTH_STENCIL) {
    return GL_UNSIGNED_INT_24_8;
  } else {
    switch (type) {
      case TexturePixelType::BYTE:
        return GL_UNSIGNED_BYTE;
      case TexturePixelType::INT:
        return GL_UNSIGNED_INT;
      case TexturePixelType::FLOAT:
        return GL_FLOAT;
    }
  }
  return GL_NONE;
}

#elif defined(_DIRECTX)

DXGI_FORMAT dg::TextureOptions::GetDirectXInternalFormat() const {
  switch (format) {
    case TexturePixelFormat::RGBA:
      switch (type) {
        case TexturePixelType::BYTE:
          return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TexturePixelType::INT:
          return DXGI_FORMAT_R32G32B32A32_UINT;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_R32G32B32A32_FLOAT;
      }
      return DXGI_FORMAT_UNKNOWN;
    case TexturePixelFormat::DEPTH:
      switch (type) {
        case TexturePixelType::BYTE:
        case TexturePixelType::INT:
          return DXGI_FORMAT_UNKNOWN;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_R32_TYPELESS;
      }
      return DXGI_FORMAT_UNKNOWN;
    case TexturePixelFormat::DEPTH_STENCIL:
      switch (type) {
        case TexturePixelType::BYTE:
          return DXGI_FORMAT_UNKNOWN;
        case TexturePixelType::INT:
          return DXGI_FORMAT_R24G8_TYPELESS;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_R32G8X24_TYPELESS;
      }
      return DXGI_FORMAT_UNKNOWN;
  }
  return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT dg::TextureOptions::GetDirectXShaderFormat() const {
  if (!shaderReadable) {
    return DXGI_FORMAT_UNKNOWN;
  }
  switch (format) {
    case TexturePixelFormat::RGBA:
      return GetDirectXInternalFormat();
    case TexturePixelFormat::DEPTH:
      switch (type) {
        case TexturePixelType::BYTE:
        case TexturePixelType::INT:
          return DXGI_FORMAT_UNKNOWN;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_R32_FLOAT;
      }
      return DXGI_FORMAT_UNKNOWN;
    case TexturePixelFormat::DEPTH_STENCIL:
      switch (type) {
        case TexturePixelType::BYTE:
          return DXGI_FORMAT_UNKNOWN;
        case TexturePixelType::INT:
          return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
      }
      return DXGI_FORMAT_UNKNOWN;
  }
  return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT dg::TextureOptions::GetDirectXDepthStencilFormat() const {
  switch (format) {
    case TexturePixelFormat::RGBA:
      return DXGI_FORMAT_UNKNOWN;
    case TexturePixelFormat::DEPTH:
      switch (type) {
        case TexturePixelType::BYTE:
        case TexturePixelType::INT:
          return DXGI_FORMAT_UNKNOWN;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_D32_FLOAT;
      }
      return DXGI_FORMAT_UNKNOWN;
    case TexturePixelFormat::DEPTH_STENCIL:
      switch (type) {
        case TexturePixelType::BYTE:
          return DXGI_FORMAT_UNKNOWN;
        case TexturePixelType::INT:
          return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
      }
      return DXGI_FORMAT_UNKNOWN;
  }
  return DXGI_FORMAT_UNKNOWN;
}

D3D11_TEXTURE_ADDRESS_MODE dg::TextureOptions::GetDirectXAddressMode() const {
  switch (wrap) {
    case TextureWrap::REPEAT:
      return D3D11_TEXTURE_ADDRESS_WRAP;
    case TextureWrap::CLAMP_EDGE:
      return D3D11_TEXTURE_ADDRESS_CLAMP;
    case TextureWrap::CLAMP_BORDER:
      return D3D11_TEXTURE_ADDRESS_BORDER;
  }
  throw EngineError(
      "Can't get D3D11_TEXTURE_ADDRESS_MODE for unknown TextureWrap: " +
      std::to_string((int)wrap));
}

D3D11_FILTER dg::TextureOptions::GetDirectXFilter() const {
  switch (interpolation) {
    case TextureInterpolation::NEAREST: {
      return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    }
    case TextureInterpolation::LINEAR: {
      return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    }
  }
  throw EngineError(
      "Can't get D3D11_FILTER for unknown TextureInterpolration: " +
      std::to_string((int)interpolation));
}

D3D11_BIND_FLAG dg::TextureOptions::GetDirectXBind() const {
  switch (format) {
    case TexturePixelFormat::DEPTH:
    case TexturePixelFormat::DEPTH_STENCIL:
      return D3D11_BIND_DEPTH_STENCIL;
      break;
    case TexturePixelFormat::RGBA:
      return D3D11_BIND_RENDER_TARGET;
      break;
  }
  throw EngineError(
      "Can't get D3D11_BIND_FLAG for unknown TexturePixelFormat: " +
      std::to_string((int)format));
}

unsigned int dg::TextureOptions::GetDirectXBitsPerPixel() const {
  DXGI_FORMAT format = GetDirectXInternalFormat();
  switch (format) {
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_D32_FLOAT:
      return 32;
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
      return 64;
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_FLOAT:
      return 32 * 3;
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
      return 32 * 4;
  }
  throw EngineError("Can't determine bits per pixel for unknown format.");
}

#endif

#pragma endregion
