//
//  Canvas.h
//

#pragma once

#include <memory>
#include <Texture.h>

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL texture resource.
  class Canvas {

    public:

      Canvas(unsigned int width, unsigned int height);
      Canvas(Canvas& other) = delete;
      Canvas(Canvas&& other);
      ~Canvas();
      Canvas& operator=(Canvas& other) = delete;
      Canvas& operator=(Canvas&& other);
      friend void swap(Canvas& first, Canvas& second); // nothrow

      std::shared_ptr<Texture> GetTexture() const;
      unsigned int GetWidth() const;
      unsigned int GetHeight() const;

      // X is from left, Y is from bottom.
      void SetPixel(
          unsigned int x, unsigned int y, GLubyte red, GLubyte green,
          GLubyte blue, GLubyte alpha = 255);

      void Submit();

    private:

      struct Pixel {
        GLubyte red;
        GLubyte green;
        GLubyte blue;
        GLubyte alpha;
      };

      Pixel *pixels = nullptr;
      std::shared_ptr<Texture> texture = nullptr;

  }; // class Canvas

} // namespace dg
