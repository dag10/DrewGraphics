//
//  Texture.h
//

#pragma once

#include <memory>
#include <string>
#include <GLUT/glut.h>

namespace dg {

class Texture {

  public:
    static std::shared_ptr<Texture> FromPath(const char *path);

    Texture();
    virtual ~Texture();

		GLuint GetHandle();

  private:
		static short le_short(unsigned char *bytes);

    GLuint texture_handle;
		uint width;
		uint height;
		char *pixels;

    void LoadFromPath(std::string path);
    void ReadTga(std::string path);

}; // class Texture

} // namespace dg
