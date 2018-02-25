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
    path.c_str(), &width, &height, &nrChannels, 0));

  if (pixels == nullptr) {
    throw dg::STBLoadError(path, stbi_failure_reason());
  }

  TextureOptions texOpts;
  texOpts.width = (unsigned int)width;
  texOpts.height = (unsigned int)height;
  texOpts.format = (nrChannels == 3) \
      ? TexturePixelFormat::RGB
      : TexturePixelFormat::RGBA;
  texOpts.type = TexturePixelType::BYTE;
  texOpts.mipmap = true;

  auto texture = std::shared_ptr<Texture>(new Texture(texOpts));
  texture->GenerateImage(pixels.get());
  return texture;
}

std::shared_ptr<dg::Texture> dg::BaseTexture::Generate(TextureOptions options) {
  auto texture = std::shared_ptr<Texture>(new Texture(options));
#if defined(_OPENGL)
  texture->GenerateImage(nullptr);
#elif defined(_DIRECTX)
  std::cerr << "WARNING: Blank textures not yet implemented for DirectX."
            << std::endl;
#endif
  return texture;
}

std::shared_ptr<dg::Texture> dg::BaseTexture::DepthTexture(
    unsigned int width, unsigned int height, bool allowStencil) {

  TextureOptions texOpts;
  texOpts.width = width;
  texOpts.height = height;
  texOpts.format = allowStencil \
      ? TexturePixelFormat::DEPTH_STENCIL
      : TexturePixelFormat::DEPTH;
  texOpts.type = TexturePixelType::INT;
  texOpts.wrap = TextureWrap::CLAMP_EDGE;
  texOpts.mipmap = false;

  return Generate(texOpts);
}

dg::BaseTexture::BaseTexture(TextureOptions options) : options(options) {}

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

  if (options.mipmap) {
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
}

void dg::DirectXTexture::GenerateImage(void *pixels) {
  assert(texture == nullptr);

  size_t pixelBytes;
  switch (options.format) {
    case TexturePixelFormat::RGB:
      pixelBytes = 3;
      break;
    case TexturePixelFormat::RGBA:
      pixelBytes = 4;
      break;
    default:
      pixelBytes = 4;
      break;
  }

  //ID3D11Resource *textureResource;
  //ID3D11ShaderResourceView *srv;

  size_t size = options.width * options.height * pixelBytes;

  //HRESULT hr = DirectX::CreateWICTextureFromMemory(
  //  Graphics::Instance->device,
  //  Graphics::Instance->context,
  //  (const uint8_t*)pixels,
  //  size,
  //  nullptr,
  //  &srv,
  //  NULL);

  //HRESULT hr = DirectX::CreateDDSTextureFromMemory(
  //  Graphics::Instance->device,
  //  Graphics::Instance->context,
  //  (const uint8_t*)pixels,
  //  size,
  //  nullptr,
  //  &srv
  //);


  unsigned int bpp = options.GetDirectXBitsPerPixel();

  //D3D11_TEXTURE2D_DESC desc;
  //desc.Width = options.width;
  //desc.Height = options.height;
  //desc.MipLevels = desc.ArraySize = 1;
  //desc.Format = options.GetDirectXFormat();
  //desc.SampleDesc.Count = 1;
  //desc.Usage = D3D11_USAGE_IMMUTABLE;
  //desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  //desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  //desc.MiscFlags = 0;

  size_t rowPitch = (options.width * bpp + 7) / 8;
  size_t imageSize = rowPitch * options.height;

  D3D11_SUBRESOURCE_DATA initData;
  initData.pSysMem = pixels;
  initData.SysMemPitch = rowPitch;
  initData.SysMemSlicePitch = imageSize;

  //HRESULT hr =
  //    Graphics::Instance->device->CreateTexture2D(&desc, &initData, &texture);

  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = desc.Height = desc.MipLevels = desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_IMMUTABLE;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

  //ID3D11Texture2D *tex;
  HRESULT hr =
      Graphics::Instance->device->CreateTexture2D(&desc, &initData, &texture);

  if (SUCCEEDED(hr)) {
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;

    hr = Graphics::Instance->device->CreateShaderResourceView(
        texture, &SRVDesc, &srv);
  }

  if (FAILED(hr)) {
    throw EngineError("Failed to load texture.");
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
      return  GL_NEAREST;
    case TextureInterpolation::LINEAR:
      return GL_LINEAR;
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLInternalFormat() const {
  switch (format) {
    case TexturePixelFormat::RGB:
      return GL_RGB;
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

DXGI_FORMAT dg::TextureOptions::GetDirectXFormat() const {
  switch (format) {
    case TexturePixelFormat::RGB: {
      switch (type) {
        case TexturePixelType::BYTE:
          return DXGI_FORMAT_R8G8B8A8_UINT; // 24 bits per pixel isn't an option
        case TexturePixelType::INT:
          return DXGI_FORMAT_R32G32B32_UINT;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_R32G32B32_FLOAT;
      }
    }
    case TexturePixelFormat::RGBA: {
      switch (type) {
        case TexturePixelType::BYTE:
          return DXGI_FORMAT_R8G8B8A8_UINT;
        case TexturePixelType::INT:
          return DXGI_FORMAT_R32G32B32A32_UINT;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_R32G32B32A32_FLOAT;
      }
    }
    case TexturePixelFormat::DEPTH:
    case TexturePixelFormat::DEPTH_STENCIL:
      break;
  }

  throw std::runtime_error(
      "Depth texture format not implemented for DirectX build.");
}

unsigned int dg::TextureOptions::GetDirectXBitsPerPixel() const {
  DXGI_FORMAT format = GetDirectXFormat();
  switch (format) {
    case DXGI_FORMAT_R8G8B8A8_UINT:
      return 8 * 4;
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

