//
//  Texture.cpp
//

#include <iostream> // TODO: TMP
#include <string>

#include "Texture.h"
#include "Exceptions.h"
#include "file_util.h"

const GLuint DEFAULT_FILTERING = GL_LINEAR;
const GLuint DEFAULT_WRAP = GL_CLAMP_TO_EDGE;

short dg::Texture::le_short(unsigned char *bytes)
{
    return bytes[0] | ((char)bytes[1] << 8);
}

std::shared_ptr<dg::Texture> dg::Texture::FromPath(const char *path) {
  auto texture = std::make_shared<dg::Texture>();
  texture->LoadFromPath(path);
  return texture;
}

void dg::Texture::LoadFromPath(std::string path) {
  ReadTga(path);

  // Generate one new texture handle.
  glGenTextures(1, &texture_handle);

  // Reference this texture through a particular target.
  glBindTexture(GL_TEXTURE_2D, texture_handle);

  // Set parameters for this texture.
  // It will use linear interpolation, and clamp out-of-bound
  // values to the nearest edge.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DEFAULT_FILTERING);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DEFAULT_FILTERING);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DEFAULT_WRAP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DEFAULT_WRAP);

  // Allocate the texture space and transfer the pixel data.
  glTexImage2D(
      GL_TEXTURE_2D,
      0, // Level of detail
      GL_RGB8, // Internal format
      width,
      height,
      0, // Border
      GL_BGR, // External format
      GL_UNSIGNED_BYTE, // Type
      pixels
      );

  std::cout << "Loaded texture: " << path << " (" << width << " x " <<
               height << ")" << std::endl;

}

void dg::Texture::ReadTga(std::string path) {

  struct tga_header {
    char  id_length;
    char  color_map_type;
    char  data_type_code;
    unsigned char  color_map_origin[2];
    unsigned char  color_map_length[2];
    char  color_map_depth;
    unsigned char  x_origin[2];
    unsigned char  y_origin[2];
    unsigned char  width[2];
    unsigned char  height[2];
    char  bits_per_pixel;
    char  image_descriptor;
  } header;

  int i;
  int color_map_size;
  int pixels_size;
  size_t read;

  FILE *f = fopen(path.c_str(), "rb");
  if (!f) {
    throw dg::FileNotFoundException(path);
  }

  read = fread(&header, 1, sizeof(header), f);

  if (read != sizeof(header)) {
    fclose(f);
    throw dg::ResourceLoadException(path + " has incomplete TGA header.");
  }

  if (header.data_type_code != 2) {
    fclose(f);
    throw dg::ResourceLoadException(
        path + " is not an uncompressed RGB TGA file.");
  }

  if (header.bits_per_pixel != 24) {
    fclose(f);
    throw dg::ResourceLoadException(
        path + " is not a 24-bit uncompressed RGB TGA file.");
  }

  for (i = 0; i < header.id_length; ++i)
  if (getc(f) == EOF) {
    fclose(f);
    throw dg::ResourceLoadException(path + " has incomplete ID string.");
  }

  color_map_size = \
      le_short(header.color_map_length) * (header.color_map_depth/8);
  for (i = 0; i < color_map_size; ++i)
  if (getc(f) == EOF) {
    fclose(f);
    throw dg::ResourceLoadException(path + " has incomplete color map.");
  }

  width = le_short(header.width);
  height = le_short(header.height);
  pixels_size = width * height * (header.bits_per_pixel/8);
  pixels = new char[pixels_size];

  read = fread(pixels, 1, pixels_size, f);
  fclose(f);

  if (read != pixels_size) {
    delete [] pixels;
    throw dg::ResourceLoadException(path + " has incomplete image.");
  }
}

dg::Texture::Texture() {
  pixels = NULL;
}

dg::Texture::~Texture() {
  if (pixels != NULL) {
    delete [] pixels;
    pixels = NULL;
  }
}

GLuint dg::Texture::GetHandle() {
  return texture_handle;
}

