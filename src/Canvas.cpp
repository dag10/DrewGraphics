//
//  Canvas.cpp
//

#include <Canvas.h>

dg::Canvas::Canvas(unsigned int width, unsigned int height) {
  texture = std::make_shared<Texture>(
      Texture::WithDimensions(width, height));

  pixels = new PixelRGB[width * height];
  memset(pixels, 0, width * height * sizeof(pixels[0]));
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
    unsigned int x, unsigned int y, GLubyte red, GLubyte green, GLubyte blue) {
  if (x >= GetWidth() || y >= GetHeight()) {
    throw std::runtime_error("Canvas SetPixel() coordinates out of bounds.");
  }

  pixels[(GetWidth() * y) + x] = {
    red = red,
    green = green,
    blue = blue,
  };
}

void dg::Canvas::Submit() {
  glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
  glTexSubImage2D(
      GL_TEXTURE_2D,
      0,
      0,
      0,
      GetWidth(),
      GetHeight(),
      GL_RGB,
      GL_UNSIGNED_BYTE,
      pixels);
}

