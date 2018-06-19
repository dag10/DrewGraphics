//
//  Canvas.cpp
//

#include "dg/Canvas.h"
#include <iostream>
#include "dg/Graphics.h"

dg::Canvas::Canvas(unsigned int width, unsigned int height) {
  TextureOptions texOpts;
  texOpts.width = width;
  texOpts.height = height;
  texOpts.interpolation = TextureInterpolation::NEAREST;
  texOpts.format = TexturePixelFormat::RGBA;
  texOpts.wrap = TextureWrap::CLAMP_EDGE;
  texOpts.pixelType = TexturePixelType::FLOAT;
  texOpts.mipmap = true;
  texture = Texture::Generate(texOpts);
  Initialize();
}

dg::Canvas::Canvas(TextureOptions textureOpts) {
  texture = Texture::Generate(textureOpts);
  Initialize();
}

void dg::Canvas::Initialize() {
  pixels = std::vector<glm::vec4>(texture->GetWidth() * texture->GetHeight());
  Submit();
}

dg::Canvas::Canvas(dg::Canvas&& other) {
  *this = std::move(other);
}

dg::Canvas& dg::Canvas::operator=(dg::Canvas&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(Canvas& first, Canvas& second) {
  using std::swap;
  swap(first.pixels, second.pixels);
  swap(first.texture, second.texture);
}

void dg::Canvas::SetPixel(unsigned int x, unsigned int y, uint8_t red,
                          uint8_t green, uint8_t blue, uint8_t alpha) {
  SetPixel(x, y, glm::vec4(red, green, blue, alpha) / 255.f);
}

void dg::Canvas::SetPixel(unsigned int x, unsigned int y, glm::vec3 value) {
  SetPixel(x, y, glm::vec4(value, 1));
}

void dg::Canvas::SetPixel(unsigned int x, unsigned int y, glm::vec4 value) {
  if (x >= GetWidth() || y >= GetHeight()) {
    throw std::runtime_error("Canvas SetPixel() coordinates out of bounds.");
  }

  pixels[(GetWidth() * y) + x] = value;
}

void dg::Canvas::Submit() {
  texture->UpdateData(pixels.data());
}
