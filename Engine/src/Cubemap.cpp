//
//  Cubemap.cpp
//

#include "dg/Cubemap.h"
#include <cassert>
#include <iostream>
#include <string>
#include "dg/Exceptions.h"
#include "dg/Graphics.h"
#include "dg/stb_image.h"

#pragma region Cubemap Base Class

std::shared_ptr<dg::Cubemap> dg::BaseCubemap::FromPaths(
    const std::string &right, const std::string &left, const std::string &top,
    const std::string &bottom, const std::string &back,
    const std::string &front) {
  TextureOptions texOpts;
  texOpts.width = 0;
  texOpts.height = 0;
  texOpts.format = TexturePixelFormat::RGBA;
  texOpts.type = TexturePixelType::BYTE;
  texOpts.interpolation = TextureInterpolation::LINEAR;
  texOpts.wrap = TextureWrap::CLAMP_EDGE;
  texOpts.mipmap = false;

  auto cubemap = std::shared_ptr<Cubemap>(new Cubemap(texOpts));
  cubemap->GenerateCubemap();
  cubemap->LoadImage(Face::Right, right);
  cubemap->LoadImage(Face::Left, left);
  cubemap->LoadImage(Face::Top, top);
  cubemap->LoadImage(Face::Bottom, bottom);
  cubemap->LoadImage(Face::Back, back);
  cubemap->LoadImage(Face::Front, front);
  return cubemap;
}

void dg::BaseCubemap::LoadImage(Face face, const std::string &filepath) {
  int nrChannels;
  int width;
  int height;

  stbi_set_flip_vertically_on_load(false);

  std::unique_ptr<stbi_uc[]> pixels = std::unique_ptr<stbi_uc[]>(
      stbi_load(filepath.c_str(), &width, &height, &nrChannels, 4));

  if (pixels == nullptr) {
    throw dg::STBLoadError(filepath, stbi_failure_reason());
  }

  options.width = (unsigned int)width;
  options.height = (unsigned int)height;

  GenerateImage(face, pixels.get());
}

std::shared_ptr<dg::Cubemap> dg::BaseCubemap::Generate(TextureOptions options) {
  auto cubemap = std::shared_ptr<Cubemap>(new Cubemap(options));
  cubemap->GenerateCubemap();
  cubemap->GenerateImage(Face::Right, nullptr);
  cubemap->GenerateImage(Face::Left, nullptr);
  cubemap->GenerateImage(Face::Top, nullptr);
  cubemap->GenerateImage(Face::Bottom, nullptr);
  cubemap->GenerateImage(Face::Back, nullptr);
  cubemap->GenerateImage(Face::Front, nullptr);
  return cubemap;
}

dg::BaseCubemap::BaseCubemap(TextureOptions options) : options(options) {
  if (options.format == TexturePixelFormat::DEPTH ||
      options.format == TexturePixelFormat::DEPTH_STENCIL) {
    if (options.type == TexturePixelType::BYTE) {
      throw EngineError(
          "Cannot create a depth[+stencil] Cubemap with byte type.");
    }
    if (options.mipmap) {
      throw EngineError(
          "Cannot create a depth[+stencil] Cubemap with mipmap enabled.");
    }
  }
  if (options.format == TexturePixelFormat::DEPTH) {
    if (options.type == TexturePixelType::INT) {
      throw EngineError(
          "Cannot create a depth-only Cubemap with int type. Must use float.");
    }
  }
}

const dg::TextureOptions dg::BaseCubemap::GetOptions() const {
  return options;
}

unsigned int dg::BaseCubemap::GetWidth() const {
  return options.width;
}

unsigned int dg::BaseCubemap::GetHeight() const {
  return options.height;
}

#pragma endregion

#pragma region OpenGL Cubemap
#if defined(_OPENGL)

GLenum dg::OpenGLCubemap::FaceToGLTarget(Face face) {
  switch (face) {
    case Face::Right:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    case Face::Left:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    case Face::Top:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    case Face::Bottom:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    case Face::Front:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    case Face::Back:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
  }
}

dg::OpenGLCubemap::OpenGLCubemap(TextureOptions options)
    : BaseCubemap(options) {}

GLuint dg::OpenGLCubemap::GetHandle() const { return cubemapHandle; }

dg::OpenGLCubemap::~OpenGLCubemap() {
  if (cubemapHandle != 0) {
    glDeleteTextures(1, &cubemapHandle);
    cubemapHandle = 0;
  }
}

void dg::OpenGLCubemap::UpdateData(Face face, const void *pixels,
                                   bool genMipMap) {
  Bind();

  glTexSubImage2D(FaceToGLTarget(face), 0, 0, 0, GetWidth(), GetHeight(),
                  options.GetOpenGLInternalFormat(), options.GetOpenGLType(),
                  pixels);

  if (options.mipmap && genMipMap) {
    GenerateMips(face);
  }

  Unbind();
}

void dg::OpenGLCubemap::GenerateMips() {
  GenerateMips(Face::Right);
  GenerateMips(Face::Left);
  GenerateMips(Face::Top);
  GenerateMips(Face::Bottom);
  GenerateMips(Face::Back);
  GenerateMips(Face::Front);
}

void dg::OpenGLCubemap::GenerateMips(Face face) {
  glGenerateMipmap(FaceToGLTarget(face));
}

void dg::OpenGLCubemap::Bind() const {
  assert(cubemapHandle != 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapHandle);
}

void dg::OpenGLCubemap::Unbind() const {
  glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);
}

void dg::OpenGLCubemap::GenerateCubemap() {
  assert(cubemapHandle == 0);
  glGenTextures(1, &cubemapHandle);

  Bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                  options.GetOpenGLMinFilter());
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                  options.GetOpenGLMagFilter());
  GLenum wrap = options.GetOpenGLWrap();
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap);
}

void dg::OpenGLCubemap::GenerateImage(Face face, void *pixels) {
  Bind();

  glTexImage2D(FaceToGLTarget(face),
               0,                                  // Level of detail
               options.GetOpenGLInternalFormat(),  // Internal format
               options.width, options.height,
               0,                                  // Border
               options.GetOpenGLExternalFormat(),  // External format
               options.GetOpenGLType(),            // Type
               pixels);

  if (pixels != nullptr && options.mipmap) {
    GenerateMips(face);
  }

  Unbind();
}

#endif
#pragma endregion
