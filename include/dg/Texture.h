//
//  Texture.h
//
#pragma once

#include <d3d11.h>

#include <memory>
#include <string>

namespace dg {

#pragma region Texture Options

  enum class TextureWrap {
    REPEAT,
    CLAMP_EDGE,
    CLAMP_BORDER,
  };

  enum class TextureInterpolation {
    NEAREST,
    LINEAR,
  };

  enum class TexturePixelFormat {
    RGBA,
    DEPTH,
    DEPTH_STENCIL,
  };

  enum class TexturePixelType {
    BYTE,
    INT,
    FLOAT,
  };

  // Options for creating a texture, generalized with my own enums.
  // The reason for creating enums values redundant to OpenGL enums is
  // to make the engine more API agnostic.
  struct TextureOptions {
    TextureWrap wrap = TextureWrap::REPEAT;

    TextureInterpolation interpolation = TextureInterpolation::LINEAR;
    TexturePixelFormat format = TexturePixelFormat::RGBA;
    TexturePixelType type = TexturePixelType::BYTE;
    bool mipmap = false;
    unsigned int width;
    unsigned int height;

    DXGI_FORMAT GetDirectXFormat() const;
    D3D11_TEXTURE_ADDRESS_MODE GetDirectXAddressMode() const;
    D3D11_FILTER GetDirectXFilter() const;
    unsigned int GetDirectXBitsPerPixel() const;
  };

#pragma endregion

  class OpenGLTexture;
  class DirectXTexture;
  using Texture = DirectXTexture;

  // Copy is disabled. This prevents us from leaking or redeleting
  // OpenGL/DirectX resources.
  class BaseTexture {

    public:

      static std::shared_ptr<Texture> FromPath(const std::string& path);
      static std::shared_ptr<Texture> Generate(TextureOptions options);
      static std::shared_ptr<Texture> DepthTexture(
          unsigned int width, unsigned int height, bool allowStencil);

      BaseTexture() = delete;

      virtual ~BaseTexture() = default;

      BaseTexture(BaseTexture& other) = delete;
      BaseTexture& operator=(BaseTexture& other) = delete;

      virtual void UpdateData(const void *pixels, bool genMipMap = true) = 0;

      const TextureOptions GetOptions() const;
      unsigned int GetWidth() const;
      unsigned int GetHeight() const;

    protected:

      BaseTexture(TextureOptions options);

      virtual void GenerateImage(void *pixels = nullptr) = 0;

      const TextureOptions options;

  }; // class BaseTexture


  class DirectXTexture : public BaseTexture {
    friend class BaseTexture;

    public:

      virtual ~DirectXTexture();

      virtual void UpdateData(const void *pixels, bool genMipMap = true);

      ID3D11ShaderResourceView *GetShaderResourceView() const;
      ID3D11SamplerState *GetSamplerState() const;
      ID3D11Texture2D *GetTexture() const;

    private:

      DirectXTexture(TextureOptions options);

      virtual void GenerateImage(void *pixels = nullptr);

      ID3D11ShaderResourceView *srv = nullptr;
      ID3D11Texture2D *texture = nullptr;
      ID3D11SamplerState *sampler = nullptr;

  }; // class DirectXTexture


} // namespace dg
