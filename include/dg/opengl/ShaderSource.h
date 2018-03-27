//
//  opengl/ShaderSource.h
//

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "dg/glad/glad.h"

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL shader resource.
  class ShaderSource {

    public:
      static ShaderSource FromFile(GLenum type, const std::string& path);
      static ShaderSource FromFileWithHeads(GLenum type,
                                            const std::string &path,
                                            std::vector<std::string> heads);

      ShaderSource() = default;
      ShaderSource(ShaderSource& other) = delete;
      ShaderSource(ShaderSource&& other);
      ~ShaderSource();
      ShaderSource& operator=(ShaderSource& other) = delete;
      ShaderSource& operator=(ShaderSource&& other);
      friend void swap(ShaderSource& first, ShaderSource& second); // nothrow

      GLuint GetHandle() const;

    private:

      void CompileShader();
      void CheckCompileErrors();

      std::vector<std::string> heads;
      std::string path = std::string();

      GLenum shaderType = 0;
      GLuint shaderHandle = 0;

  }; // class ShaderSource

} // namespace dg
