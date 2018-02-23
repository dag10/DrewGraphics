//
//  opengl/ShaderSource.h
//

#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL shader resource.
  class ShaderSource {

    public:
      static ShaderSource FromFile(GLenum type, const std::string& path);
      static ShaderSource FromFileWithHead(
          GLenum type, const std::string& path, const std::string& headCode);

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

      const char *headCode = nullptr;
      std::string path = std::string();

      GLenum shaderType = 0;
      GLuint shaderHandle = 0;

  }; // class ShaderSource

} // namespace dg

