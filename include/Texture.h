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
      static Texture FromPath(const char *path);

      Texture() = default;
      Texture(Texture& other) = delete;
      Texture(Texture&& other);
      ~Texture();
      Texture& operator=(Texture& other) = delete;
      Texture& operator=(Texture&& other);
      friend void swap(Texture& first, Texture& second); // nothrow

      GLuint GetHandle();

    private:
      static inline short le_short(unsigned char *bytes);
      static std::unique_ptr<char[]> ReadTga(
          std::string path, int *width, int *height);

      void LoadFromPath(std::string path);

      GLuint texture_handle = 0;
      int width = 0;
      int height = 0;
  }; // class Texture

} // namespace dg
