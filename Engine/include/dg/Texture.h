//
//  Texture.h
//
#pragma once

#if defined(_OPENGL)
#include "dg/opengl/glad/glad.h"
#elif defined(_DIRECTX)
#include <d3d11.h>
#endif

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
    bool shaderReadable = true;
    bool cpuReadable = false;
    unsigned int width;
    unsigned int height;

#if defined(_OPENGL)
    GLenum GetOpenGLWrap() const;
    GLenum GetOpenGLMinFilter() const;
    GLenum GetOpenGLMagFilter() const;
    GLenum GetOpenGLInternalFormat() const;
    GLenum GetOpenGLExternalFormat() const;
    GLenum GetOpenGLType() const;
#elif defined(_DIRECTX)
    DXGI_FORMAT GetDirectXInternalFormat() const;
    DXGI_FORMAT GetDirectXShaderFormat() const;
    DXGI_FORMAT GetDirectXDepthStencilFormat() const;
    D3D11_TEXTURE_ADDRESS_MODE GetDirectXAddressMode() const;
    D3D11_FILTER GetDirectXFilter() const;
    D3D11_BIND_FLAG GetDirectXBind() const;
    unsigned int GetDirectXBitsPerPixel() const;
#endif
  };

#pragma endregion

  class OpenGLTexture;
  class DirectXTexture;
#if defined(_OPENGL)
  using Texture = OpenGLTexture;
#elif defined(_DIRECTX)
  using Texture = DirectXTexture;
#endif

  // Copy is disabled. This prevents us from leaking or redeleting
  // OpenGL/DirectX resources.
  class BaseTexture {

    public:

      static std::shared_ptr<Texture> FromPath(const std::string& path);
      static std::shared_ptr<Texture> Generate(TextureOptions options);

      BaseTexture() = delete;

      virtual ~BaseTexture() = default;

      BaseTexture(BaseTexture& other) = delete;
      BaseTexture& operator=(BaseTexture& other) = delete;

      virtual void UpdateData(const void *pixels, bool genMipMap = true) = 0;
      virtual void GenerateMips() = 0;

      const TextureOptions GetOptions() const;
      unsigned int GetWidth() const;
      unsigned int GetHeight() const;

    protected:

      BaseTexture(TextureOptions options);

      virtual void GenerateImage(void *pixels = nullptr) = 0;

      const TextureOptions options;

  }; // class BaseTexture

#if defined(_OPENGL)

  class OpenGLTexture : public BaseTexture {
    friend class BaseTexture;

    public:

      virtual ~OpenGLTexture();

      virtual void UpdateData(const void *pixels, bool genMipMap = true);
      virtual void GenerateMips();

      GLuint GetHandle() const;

    private:

      OpenGLTexture(TextureOptions options);

      virtual void GenerateImage(void *pixels);

      GLuint textureHandle = 0;

  }; // class OpenGLTexture

#elif defined(_DIRECTX)

  class DirectXTexture : public BaseTexture {
    friend class BaseTexture;

    public:

      virtual ~DirectXTexture();

      virtual void UpdateData(const void *pixels, bool genMipMap = true);
      virtual void GenerateMips();

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

#endif

} // namespace dg
