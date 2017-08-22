//
//  Texture.cpp
//

#include <string>
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture.h"
#include "Exceptions.h"

const GLuint DEFAULT_FILTERING = GL_LINEAR;
const GLuint DEFAULT_WRAP = GL_CLAMP_TO_EDGE;

dg::Texture dg::Texture::FromPath(const char *path) {
  dg::Texture tex;
  tex.LoadFromPath(path);
  return tex;
}

GLuint dg::Texture::GetHandle() const {
  return textureHandle;
}

dg::Texture::Texture(dg::Texture&& other) {
  *this = std::move(other);
}

dg::Texture::~Texture() {
  if (textureHandle != 0) {
    glDeleteTextures(1, &textureHandle);
    textureHandle = 0;
  }
}

dg::Texture& dg::Texture::operator=(dg::Texture&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(Texture& first, Texture& second) {
  using std::swap;
  swap(first.textureHandle, second.textureHandle);
  swap(first.width, second.width);
  swap(first.height, second.height);
}

void dg::Texture::LoadFromPath(std::string path) {
  assert(textureHandle == 0);

  stbi_set_flip_vertically_on_load(true);
  int nrChannels;
  std::unique_ptr<stbi_uc[]> pixels = std::unique_ptr<stbi_uc[]>(stbi_load(
      path.c_str(), &width, &height, &nrChannels, 0));

  // Generate one new texture handle.
  glGenTextures(1, &textureHandle);

  // Reference this texture through a particular target.
  glBindTexture(GL_TEXTURE_2D, textureHandle);

  // Set parameters for this texture.
  // It will use linear interpolation, and clamp out-of-bound
  // values to the nearest edge.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DEFAULT_WRAP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DEFAULT_WRAP);

  // Allocate the texture space and transfer the pixel data.
  GLenum externalFormat = (nrChannels == 3) ? GL_RGB : GL_RGBA;
  glTexImage2D(
      GL_TEXTURE_2D,
      0, // Level of detail
      GL_RGB, // Internal format
      width,
      height,
      0, // Border
      externalFormat, // External format
      GL_UNSIGNED_BYTE, // Type
      pixels.get()
      );

  glGenerateMipmap(GL_TEXTURE_2D);
}

