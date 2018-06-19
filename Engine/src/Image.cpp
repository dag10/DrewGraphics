//
//  Image.cpp
//

#define STB_IMAGE_IMPLEMENTATION

#include "dg/Image.h"
#include "dg/Exceptions.h"
#include "dg/stb_image.h"

std::shared_ptr<dg::Image> dg::Image::FromPath(const std::string &path,
                                               bool flipVertically) {
  stbi_set_flip_vertically_on_load(flipVertically);

  int nrChannels;
  int width;
  int height;
  auto pixels = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

  if (pixels == nullptr) {
    throw dg::STBLoadError(path, stbi_failure_reason());
  }

  auto img = std::shared_ptr<Image>(new Image());
  img->path = path;
  img->width = (unsigned int)width;
  img->height = (unsigned int)height;
  img->pixels = pixels;
  return img;
}

dg::Image::~Image() {
  if (pixels != nullptr) {
    delete[] pixels;
    pixels = nullptr;
  }
}

const std::string &dg::Image::GetPath() const { return path; }

unsigned char *dg::Image::GetPixels() const { return pixels; }

unsigned int dg::Image::GetWidth() const { return width; }

unsigned int dg::Image::GetHeight() const { return height; }
