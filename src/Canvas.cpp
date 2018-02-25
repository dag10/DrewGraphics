//
//  Canvas.cpp
//

#include "dg/Canvas.h"
#include <iostream>

dg::Canvas::Canvas(unsigned int width, unsigned int height) {
  TextureOptions texOpts;
  texOpts.width = width;
  texOpts.height = height;
  texOpts.interpolation = TextureInterpolation::NEAREST;
  texOpts.format = TexturePixelFormat::RGBA;
  texOpts.wrap = TextureWrap::CLAMP_EDGE;
  texOpts.type = TexturePixelType::BYTE;
  texture = Texture::Generate(texOpts);

  pixels = new Pixel[width * height]();
  Submit();
}

dg::Canvas::Canvas(dg::Canvas&& other) {
  *this = std::move(other);
}

dg::Canvas::~Canvas() {
  delete [] pixels;
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

std::shared_ptr<dg::Texture> dg::Canvas::GetTexture() const {
  return texture;
}

unsigned int dg::Canvas::GetWidth() const {
  return texture->GetWidth();
}

unsigned int dg::Canvas::GetHeight() const {
  return texture->GetHeight();
}

void dg::Canvas::SetPixel(
    unsigned int x, unsigned int y,
    uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
  if (x >= GetWidth() || y >= GetHeight()) {
    throw std::runtime_error("Canvas SetPixel() coordinates out of bounds.");
  }

  Pixel *pixel = pixels + (GetWidth() * y) + x;
  pixel->red = red;
  pixel->green = green;
  pixel->blue = blue;
  pixel->alpha = alpha;
}

void dg::Canvas::Submit() {
#if defined(_OPENGL)
  glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
  glTexSubImage2D(
      GL_TEXTURE_2D,
      0,
      0,
      0,
      GetWidth(),
      GetHeight(),
      texture->GetOptions().GetOpenGLInternalFormat(),
      texture->GetOptions().GetOpenGLType(),
      pixels);
#elif defined(_DIRECTX)
  // TODO
#endif
}

