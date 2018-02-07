//
//  Texture.h
//
#pragma once

#include <memory>
#include <string>
#include <glad/glad.h>

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL texture resource.
  class Texture {

    public:
      static Texture FromPath(const std::string& path);
      static Texture WithDimensions(unsigned int width, unsigned int height);
      static Texture DepthTexture(
        unsigned int width, unsigned int height, bool allowStencil);

      Texture() = default;
      Texture(Texture& other) = delete;
      Texture(Texture&& other);
      ~Texture();
      Texture& operator=(Texture& other) = delete;
      Texture& operator=(Texture&& other);
      friend void swap(Texture& first, Texture& second); // nothrow

      GLuint GetHandle() const;
      unsigned int GetWidth() const;
      unsigned int GetHeight() const;

    private:
      void LoadFromPath(const std::string& path);
      void GenerateImage(
        GLenum internalFormat, void *pixels, GLenum externalFormat,
        GLenum externalType, bool mipmap);

      GLuint textureHandle = 0;
      int width = 0;
      int height = 0;
  }; // class Texture

} // namespace dg
