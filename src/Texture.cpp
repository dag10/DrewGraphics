//
//  Texture.cpp
//

#include <string>
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Texture.h>
#include <Exceptions.h>

#pragma region Texture

dg::Texture dg::Texture::FromPath(const std::string& path) {
  int nrChannels;
  int width;
  int height;

  stbi_set_flip_vertically_on_load(true);
  std::unique_ptr<stbi_uc[]> pixels = std::unique_ptr<stbi_uc[]>(stbi_load(
    path.c_str(), &width, &height, &nrChannels, 0));

  if (pixels == nullptr) {
    throw dg::STBLoadError(path, stbi_failure_reason());
  }

  TextureOptions texOpts;
  texOpts.width = (unsigned int)width;
  texOpts.height = (unsigned int)height;
  texOpts.format = (nrChannels == 3) \
      ? TexturePixelFormat::RGB
      : TexturePixelFormat::RGBA;
  texOpts.type = TexturePixelType::BYTE;
  texOpts.mipmap = true;

  Texture tex(texOpts, false);
  tex.GenerateImage(pixels.get());
  return tex;
}

dg::Texture dg::Texture::DepthTexture(
  unsigned int width, unsigned int height, bool allowStencil) {

  TextureOptions texOpts;
  texOpts.width = width;
  texOpts.height = height;
  texOpts.format = allowStencil \
      ? TexturePixelFormat::DEPTH_STENCIL
      : TexturePixelFormat::DEPTH;
  texOpts.type = TexturePixelType::INT;
  texOpts.wrap = TextureWrap::CLAMP_EDGE;
  texOpts.mipmap = false;

  return Texture(texOpts);
}

dg::Texture::Texture(TextureOptions options) : Texture(options, true) {}

dg::Texture::Texture(TextureOptions options, bool generate) : options(options) {
  if (generate) {
    GenerateImage();
  }
}

GLuint dg::Texture::GetHandle() const {
  return textureHandle;
}

const dg::TextureOptions dg::Texture::GetOptions() const {
  return options;
}

unsigned int dg::Texture::GetWidth() const {
  return options.width;
}

unsigned int dg::Texture::GetHeight() const {
  return options.height;
}

dg::Texture::Texture(dg::Texture&& other) : options() {
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
  swap((TextureOptions&)first.options, (TextureOptions&)second.options);
}

void dg::Texture::GenerateImage(void *pixels) {
  assert(textureHandle == 0);

  glGenTextures(1, &textureHandle);

  glBindTexture(GL_TEXTURE_2D, textureHandle);

  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options.GetOpenGLMinFilter());
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options.GetOpenGLMagFilter());
  GLenum wrap = options.GetOpenGLWrap();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

  glTexImage2D(
      GL_TEXTURE_2D,
      0, // Level of detail
      options.GetOpenGLInternalFormat(), // Internal format
      options.width,
      options.height,
      0, // Border
      options.GetOpenGLExternalFormat(), // External format
      options.GetOpenGLType(), // Type
      pixels
      );

  if (options.mipmap) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

#pragma endregion

#pragma region TextureOptions

GLenum dg::TextureOptions::GetOpenGLWrap() const {
  switch (wrap) {
    case TextureWrap::REPEAT:
      return GL_REPEAT;
    case TextureWrap::CLAMP_EDGE:
      return GL_CLAMP_TO_EDGE;
    case TextureWrap::CLAMP_BORDER:
      return GL_CLAMP_TO_BORDER;
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLMinFilter() const {
  // Minification filter. Will be the same as the magnification filter,
  // but if mipmap is requested, will also linearly interpolate between
  // the two closest mip levels.
  if (mipmap) {
    switch (interpolation) {
      case TextureInterpolation::NEAREST:
        return GL_NEAREST_MIPMAP_LINEAR;
      case TextureInterpolation::LINEAR:
        return GL_LINEAR_MIPMAP_LINEAR;
    }
  } else {
    return GetOpenGLMagFilter();
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLMagFilter() const {
  switch (interpolation) {
    case TextureInterpolation::NEAREST:
      return  GL_NEAREST;
    case TextureInterpolation::LINEAR:
      return GL_LINEAR;
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLInternalFormat() const {
  switch (format) {
    case TexturePixelFormat::RGB:
      return GL_RGB;
    case TexturePixelFormat::RGBA:
      return GL_RGBA;
    case TexturePixelFormat::DEPTH:
      return GL_DEPTH_COMPONENT;
    case TexturePixelFormat::DEPTH_STENCIL:
      return GL_DEPTH24_STENCIL8;
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLExternalFormat() const {
  switch (format) {
    case TexturePixelFormat::DEPTH_STENCIL:
      return GL_DEPTH_STENCIL;
    default:
      return GetOpenGLInternalFormat();
  }
  return GL_NONE;
}

GLenum dg::TextureOptions::GetOpenGLType() const {
  if (format == TexturePixelFormat::DEPTH_STENCIL) {
    return GL_UNSIGNED_INT_24_8;
  } else {
    switch (type) {
      case TexturePixelType::BYTE:
        return GL_UNSIGNED_BYTE;
      case TexturePixelType::INT:
        return GL_UNSIGNED_INT;
      case TexturePixelType::FLOAT:
        return GL_FLOAT;
    }
  }
  return GL_NONE;
}

#pragma endregion

