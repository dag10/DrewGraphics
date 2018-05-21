//
//  opengl/ShaderSource.h
//

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "dg/Preprocessor.h"
#include "dg/opengl/glad/glad.h"

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL shader resource.
  class ShaderSource {

    public:

      static std::shared_ptr<ShaderSource> FromFile(GLenum type,
                                                    const std::string& path);

      ShaderSource() = default;
      ShaderSource(ShaderSource& other) = delete;
      ~ShaderSource();
      ShaderSource& operator=(ShaderSource& other) = delete;

      inline GLuint GetHandle() const {
        return shaderHandle;
      }
      inline const std::string &GetPath() const {
        return file->GetFilename();
      }
      inline const std::string &GetContent() const {
        return file->GetProcessedContent();
      }

    private:

      void CompileShader();
      void CheckCompileErrors();

      std::shared_ptr<Preprocessor> file;

      GLenum shaderType = 0;
      GLuint shaderHandle = 0;

  }; // class ShaderSource

} // namespace dg
