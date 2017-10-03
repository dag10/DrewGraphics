//
//  Texture.h
//
#pragma once

#include <memory>
#include <string>
#include <GLUT/glut.h>

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL texture resource.
  class Texture {

    public:
      static Texture FromPath(const std::string& path);

      Texture() = default;
      Texture(Texture& other) = delete;
      Texture(Texture&& other);
      ~Texture();
      Texture& operator=(Texture& other) = delete;
      Texture& operator=(Texture&& other);
      friend void swap(Texture& first, Texture& second); // nothrow

      GLuint GetHandle() const;

    private:
      void LoadFromPath(const std::string& path);

      GLuint textureHandle = 0;
      int width = 0;
      int height = 0;
  }; // class Texture

} // namespace dg
