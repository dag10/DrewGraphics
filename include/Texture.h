//
//  Texture.h
//
#pragma once

#include <memory>
#include <string>
#include <glad/glad.h>

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
    RGB,
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

    GLenum GetOpenGLWrap() const;
    GLenum GetOpenGLMinFilter() const;
    GLenum GetOpenGLMagFilter() const;
    GLenum GetOpenGLInternalFormat() const;
    GLenum GetOpenGLExternalFormat() const;
    GLenum GetOpenGLType() const;
  };

#pragma endregion

  class OpenGLTexture;
  using Texture = OpenGLTexture;

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

      GLuint GetHandle() const;

    private:

      OpenGLTexture(TextureOptions options);

      virtual void GenerateImage(void *pixels = nullptr);

      GLuint textureHandle = 0;

  }; // class OpenGLTexture

#endif

} // namespace dg
