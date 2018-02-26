//
//  Texture.cpp
//

#include <DDSTextureLoader.h>

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
#pragma endregion
#pragma region DirectX Texture

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
                                                 rowPitch, 0);
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

  auto format = options.GetDirectXFormat();

  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = options.width;
  desc.Height = options.height;
  desc.MipLevels = options.mipmap ? 0 : 1;
  desc.ArraySize = 1;
  desc.Format = format;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  if (options.mipmap) {
    desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
  }

  HRESULT hr =
      Graphics::Instance->device->CreateTexture2D(&desc, nullptr, &texture);

  if (FAILED(hr)) {
    throw EngineError("Failed to load texture.");
  }

  if (pixels != nullptr) {
    UpdateData(pixels, false);
  }

  D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
  SRVDesc.Format = format;
  SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  SRVDesc.Texture2D.MipLevels = -1;

  hr = Graphics::Instance->device->CreateShaderResourceView(texture, &SRVDesc,
                                                            &srv);

  if (FAILED(hr)) {
    throw EngineError("Failed to create shader resource view.");
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

  if (options.mipmap) {
    Graphics::Instance->context->GenerateMips(srv);
  }
}

#pragma endregion

#pragma region TextureOptions


DXGI_FORMAT dg::TextureOptions::GetDirectXFormat() const {
  switch (format) {
    case TexturePixelFormat::RGBA: {
      switch (type) {
        case TexturePixelType::BYTE:
          return DXGI_FORMAT_R8G8B8A8_UNORM;
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

D3D11_TEXTURE_ADDRESS_MODE dg::TextureOptions::GetDirectXAddressMode() const {
  switch (wrap) {
    case TextureWrap::REPEAT:
      return D3D11_TEXTURE_ADDRESS_WRAP;
    case TextureWrap::CLAMP_EDGE:
      return D3D11_TEXTURE_ADDRESS_CLAMP;
    case TextureWrap::CLAMP_BORDER:
      return D3D11_TEXTURE_ADDRESS_BORDER;
  }
  throw EngineError("Texture address mode is not implemented for DirectX.");
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
  throw EngineError("Texture filter mode is not implemented for DirectX.");
}

unsigned int dg::TextureOptions::GetDirectXBitsPerPixel() const {
  DXGI_FORMAT format = GetDirectXFormat();
  switch (format) {
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
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


#pragma endregion

