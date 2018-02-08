//
//  Texture.h
//
#pragma once

#include <memory>
#include <string>
#include <glad/glad.h>

namespace dg {

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

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL texture resource.
  class Texture {

    public:

      static Texture FromPath(const std::string& path);
      static Texture DepthTexture(
        unsigned int width, unsigned int height, bool allowStencil);

      Texture(TextureOptions options);
      Texture(Texture& other) = delete;
      Texture(Texture&& other);
      Texture() = delete;
      virtual ~Texture();
      Texture& operator=(Texture& other) = delete;
      Texture& operator=(Texture&& other);
      friend void swap(Texture& first, Texture& second); // nothrow

      GLuint GetHandle() const;
      const TextureOptions GetOptions() const;
      unsigned int GetWidth() const;
      unsigned int GetHeight() const;

    private:

      Texture(TextureOptions options, bool generate);

      void GenerateImage(void *pixels = nullptr);

      GLuint textureHandle = 0;
      const TextureOptions options;

  }; // class Texture

} // namespace dg
