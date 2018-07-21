//
//  Texture.cpp
//

#include "dg/Texture.h"
#include <cassert>
#include <iostream>
#include <string>
#include "dg/Exceptions.h"
#include "dg/Graphics.h"
#include "dg/Image.h"

#pragma region Texture Base Class

dg::Transform dg::BaseTexture::TransformForFace(TextureFace face) {
  switch (face) {
    case TextureFace::Right:
      return Transform::R(glm::quat(glm::radians(glm::vec3(0, -90, 0))));
    case TextureFace::Left:
      return Transform::R(glm::quat(glm::radians(glm::vec3(0, 90, 0))));
    case TextureFace::Top:
      return Transform::R(glm::quat(glm::radians(glm::vec3(90, 0, 0))));
    case TextureFace::Bottom:
      return Transform::R(glm::quat(glm::radians(glm::vec3(-90, 0, 0))));
    case TextureFace::Back:
      return Transform::R(glm::quat(glm::radians(glm::vec3(0, 180, 0))));
    case TextureFace::Front:
      return Transform();
  }
}

std::shared_ptr<dg::Texture> dg::BaseTexture::FromPath(
    const std::string &path) {
  return FromImage(Image::FromPath(path));
}

std::shared_ptr<dg::Texture> dg::BaseTexture::FromImage(
    std::shared_ptr<Image> image) {
  TextureOptions texOpts;
  texOpts.width = image->GetWidth();
  texOpts.height = image->GetHeight();
  texOpts.format = TexturePixelFormat::RGBA;
  texOpts.pixelType = TexturePixelType::BYTE;
  texOpts.mipmap = true;

  auto texture = std::shared_ptr<Texture>(new Texture(texOpts));
  texture->GenerateImage(image->GetPixels());
  return texture;
}

std::shared_ptr<dg::Texture> dg::BaseTexture::FromPaths(
    const std::string &right, const std::string &left, const std::string &top,
    const std::string &bottom, const std::string &back,
    const std::string &front) {
  return FromImages(Image::FromPath(right, false), Image::FromPath(left, false),
                    Image::FromPath(top, false), Image::FromPath(bottom, false),
                    Image::FromPath(back, false),
                    Image::FromPath(front, false));
}

std::shared_ptr<dg::Texture> dg::BaseTexture::FromImages(
    std::shared_ptr<Image> right, std::shared_ptr<Image> left,
    std::shared_ptr<Image> top, std::shared_ptr<Image> bottom,
    std::shared_ptr<Image> back, std::shared_ptr<Image> front) {

  // Ensure all cubemap face images are the same dimensions.
  std::shared_ptr<Image> images[] = {right, left, top, bottom, back, front};
  unsigned int width = images[0]->GetWidth();
  unsigned int height = images[0]->GetHeight();
  for (int i = 1; i < 6; i++) {
    if (images[i]->GetWidth() != width || images[i]->GetHeight() != height) {
      throw EngineError(
          "Cannot create a cubemap Texture with images of unequal dimensions.");
    }
  }

  TextureOptions texOpts;
  texOpts.type = TextureType::CUBEMAP;
  texOpts.width = width;
  texOpts.height = height;
  texOpts.format = TexturePixelFormat::RGBA;
  texOpts.pixelType = TexturePixelType::BYTE;
  texOpts.interpolation = TextureInterpolation::LINEAR;
  texOpts.wrap = TextureWrap::CLAMP_EDGE;
  texOpts.mipmap = false;

  auto texture = Texture::Generate(texOpts);
  texture->UpdateData(TextureFace::Right, right->GetPixels(), true);
  texture->UpdateData(TextureFace::Left, left->GetPixels(), true);
  texture->UpdateData(TextureFace::Top, top->GetPixels(), true);
  texture->UpdateData(TextureFace::Bottom, bottom->GetPixels(), true);
  texture->UpdateData(TextureFace::Back, back->GetPixels(), true);
  texture->UpdateData(TextureFace::Front, front->GetPixels(), true);
  return texture;
}

std::shared_ptr<dg::Texture> dg::BaseTexture::Generate(TextureOptions options) {
  auto texture = std::shared_ptr<Texture>(new Texture(options));
  texture->GenerateImage(nullptr);
  return texture;
}

dg::BaseTexture::BaseTexture(TextureOptions options) : options(options) {
  if (options.format == TexturePixelFormat::DEPTH ||
      options.format == TexturePixelFormat::DEPTH_STENCIL) {
    if (options.pixelType == TexturePixelType::BYTE) {
      throw EngineError(
          "Cannot create a depth[+stencil] Texture with byte type.");
    }
    if (options.mipmap) {
      throw EngineError(
          "Cannot create a depth[+stencil] Texture with mipmap enabled.");
    }
  }
  if (options.format == TexturePixelFormat::DEPTH) {
    if (options.pixelType == TexturePixelType::INT) {
      throw EngineError(
          "Cannot create a depth-only Texture with int type. Must use float.");
    }
  }
}

const dg::TextureOptions dg::BaseTexture::GetOptions() const {
  return options;
}

dg::TextureType dg::BaseTexture::GetType() const {
  return options.type;
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

GLenum dg::OpenGLTexture::FaceToGLTarget(TextureFace face) {
  switch (face) {
    case TextureFace::Right:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    case TextureFace::Left:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    case TextureFace::Top:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    case TextureFace::Bottom:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    case TextureFace::Front:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    case TextureFace::Back:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
  }
}

dg::OpenGLTexture::OpenGLTexture(TextureOptions options)
    : BaseTexture(options) {}

GLuint dg::OpenGLTexture::GetHandle() const { return textureHandle; }

dg::OpenGLTexture::~OpenGLTexture() {
  if (textureHandle != 0) {
    glDeleteTextures(1, &textureHandle);
    textureHandle = 0;
  }
}

void dg::OpenGLTexture::Bind() const {
  assert(textureHandle != 0);
  glBindTexture(options.GetOpenGLTarget(), textureHandle);
}

void dg::OpenGLTexture::Unbind() const {
  glBindTexture(options.GetOpenGLTarget(), GL_NONE);
}

void dg::OpenGLTexture::UpdateData(const void *pixels, bool genMipMap) {
  Bind();
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
    GenerateMips();
  }
  Unbind();
}

void dg::OpenGLTexture::UpdateData(TextureFace face, const void *pixels,
                                   bool genMipMap) {
  Bind();

  glTexImage2D(FaceToGLTarget(face),
               0,                                  // Level of detail
               options.GetOpenGLInternalFormat(),  // Internal format
               options.width, options.height,
               0,                                  // Border
               options.GetOpenGLExternalFormat(),  // External format
               options.GetOpenGLType(),            // Type
               pixels);

  if (pixels != nullptr && options.mipmap) {
    GenerateMips(face);
  }

  Unbind();
}

void dg::OpenGLTexture::GenerateMips() {
  switch (GetType()) {
    case TextureType::_2D:
      glGenerateMipmap(GL_TEXTURE_2D);
      break;
    case TextureType::CUBEMAP:
      GenerateMips(TextureFace::Right);
      GenerateMips(TextureFace::Left);
      GenerateMips(TextureFace::Top);
      GenerateMips(TextureFace::Bottom);
      GenerateMips(TextureFace::Back);
      GenerateMips(TextureFace::Front);
      break;
  }
}

void dg::OpenGLTexture::GenerateMips(TextureFace face) {
  glGenerateMipmap(FaceToGLTarget(face));
}

void dg::OpenGLTexture::GenerateImage(void *pixels) {
  assert(textureHandle == 0);

  GLenum target = options.GetOpenGLTarget();

  glGenTextures(1, &textureHandle);
  glBindTexture(target, textureHandle);

  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, options.GetOpenGLMinFilter());
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, options.GetOpenGLMagFilter());
  GLenum wrap = options.GetOpenGLWrap();
  glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
  if (options.type == TextureType::CUBEMAP) {
    glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
  }

  switch (options.type) {
    case TextureType::_2D:
      glTexImage2D(GL_TEXTURE_2D,
                   0,                                  // Level of detail
                   options.GetOpenGLInternalFormat(),  // Internal format
                   options.width,
                   options.height,
                   0,                                  // Border
                   options.GetOpenGLExternalFormat(),  // External format
                   options.GetOpenGLType(),            // Type
                   pixels);
      break;
    case TextureType::CUBEMAP:
      for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,                                  // Level of detail
                     options.GetOpenGLInternalFormat(),  // Internal format
                     options.width,
                     options.height,
                     0,                                  // Border
                     options.GetOpenGLExternalFormat(),  // External format
                     options.GetOpenGLType(),            // Type
                     pixels);
      }
      break;
  }

  if (pixels != nullptr && options.mipmap) {
    glGenerateMipmap(target);
  }

  glBindTexture(target, 0);
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
    GenerateMips();
  }
}

void dg::DirectXTexture::UpdateData(TextureFace face, const void *pixels,
                                    bool genMipMap) {
  throw EngineError("TODO: Implement DirectX cubemap textures.");
}

void dg::DirectXTexture::GenerateMips() {
  Graphics::Instance->context->GenerateMips(srv);
}

void dg::DirectXTexture::GenerateMips(TextureFace face) {
  throw EngineError("TODO: Implement DirectX cubemap textures.");
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

  if (options.cpuReadable && options.shaderReadable) {
    throw EngineError(
        "Cannot create a texture that is CPU readable and shader readable.");
  }

  auto internalFormat = options.GetDirectXInternalFormat();

  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = options.width;
  desc.Height = options.height;
  desc.MipLevels = options.mipmap ? 0 : 1;
  desc.ArraySize = 1;
  desc.Format = internalFormat;
  desc.SampleDesc.Count = 1;
  if (options.cpuReadable) {
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
  } else {
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = options.GetDirectXBind();
  }
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

GLenum dg::TextureOptions::GetOpenGLTarget() const {
  switch (type) {
    case TextureType::_2D:
      return GL_TEXTURE_2D;
    case TextureType::CUBEMAP:
      return GL_TEXTURE_CUBE_MAP;
  }
}

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
    switch (pixelType) {
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
      switch (pixelType) {
        case TexturePixelType::BYTE:
          return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TexturePixelType::INT:
          return DXGI_FORMAT_R32G32B32A32_UINT;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_R32G32B32A32_FLOAT;
      }
      return DXGI_FORMAT_UNKNOWN;
    case TexturePixelFormat::DEPTH:
      switch (pixelType) {
        case TexturePixelType::BYTE:
        case TexturePixelType::INT:
          return DXGI_FORMAT_UNKNOWN;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_R32_TYPELESS;
      }
      return DXGI_FORMAT_UNKNOWN;
    case TexturePixelFormat::DEPTH_STENCIL:
      switch (pixelType) {
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
      switch (pixelType) {
        case TexturePixelType::BYTE:
        case TexturePixelType::INT:
          return DXGI_FORMAT_UNKNOWN;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_R32_FLOAT;
      }
      return DXGI_FORMAT_UNKNOWN;
    case TexturePixelFormat::DEPTH_STENCIL:
      switch (pixelType) {
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
      switch (pixelType) {
        case TexturePixelType::BYTE:
        case TexturePixelType::INT:
          return DXGI_FORMAT_UNKNOWN;
        case TexturePixelType::FLOAT:
          return DXGI_FORMAT_D32_FLOAT;
      }
      return DXGI_FORMAT_UNKNOWN;
    case TexturePixelFormat::DEPTH_STENCIL:
      switch (pixelType) {
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
