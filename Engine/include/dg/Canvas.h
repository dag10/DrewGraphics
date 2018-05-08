//
//  Canvas.h
//

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "dg/Texture.h"

namespace dg {

// Copy is disabled, only moves are allowed. This prevents us
// from leaking or redeleting the openGL texture resource.
class Canvas {
  public:
  Canvas(unsigned int width, unsigned int height);
  Canvas(TextureOptions textureOpts);
  Canvas(Canvas &other) = delete;
  Canvas(Canvas &&other);
  virtual ~Canvas() {};
  Canvas &operator=(Canvas &other) = delete;
  Canvas &operator=(Canvas &&other);
  friend void swap(Canvas &first, Canvas &second);  // nothrow

  inline std::shared_ptr<Texture> GetTexture() const { return texture; }
  inline unsigned int GetWidth() const { return texture->GetWidth(); }
  inline unsigned int GetHeight() const { return texture->GetHeight(); }

  // X is from left, Y is from bottom.
  void SetPixel(unsigned int x, unsigned int y, uint8_t red, uint8_t green,
                uint8_t blue, uint8_t alpha = 255);
  void SetPixel(unsigned int x, unsigned int y, glm::vec3 value);
  void SetPixel(unsigned int x, unsigned int y, glm::vec4 value);

  void Submit();

  private:

  void Initialize();

  std::vector<glm::vec4> pixels;
  std::shared_ptr<Texture> texture = nullptr;

};  // class Canvas

}  // namespace dg
