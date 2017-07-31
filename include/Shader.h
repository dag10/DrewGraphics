//
//  Shader
//

#pragma once

#include <string>
#include <GLUT/glut.h>
#include <glm/glm.hpp>
#include "Texture.h"

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL shader program resource.
  class Shader {

    public:
      static Shader FromFiles(
          const std::string& vertexPath, const std::string& fragmentPath);

      Shader() = default;
      Shader(Shader& other) = delete;
      Shader(Shader&& other);
      ~Shader();
      Shader& operator=(Shader& other) = delete;
      Shader& operator=(Shader&& other);
      friend void swap(Shader& first, Shader& second); // nothrow

      void Use();
      
      GLint GetUniformLocation(const std::string& name) const;
      GLint GetAttributeLocation(const std::string& name) const;

      void SetBool(const std::string& name, bool value) const;
      void SetInt(const std::string& name, int value) const;
      void SetFloat(const std::string& name, float value) const;
      void SetVec2(const std::string& name, const glm::vec2& value) const;
      void SetVec2(const std::string& name, float x, float y) const;
      void SetVec3(const std::string& name, const glm::vec3& value) const;
      void SetVec3(const std::string& name, float x, float y, float z) const;
      void SetVec4(const std::string& name, const glm::vec4& value) const;
      void SetVec4(const std::string& name, float x, float y, float z, float w) ;
      void SetMat2(const std::string& name, const glm::mat2& mat) const;
      void SetMat3(const std::string& name, const glm::mat3& mat) const;
      void SetMat4(const std::string& name, const glm::mat4& mat) const;
      void SetTexture(
          unsigned int textureUnit, const std::string& name,
          const dg::Texture& texture) const;

    private:
      void CreateProgram();
      void CheckLinkErrors();

      std::string vertexPath = std::string();
      std::string fragmentPath = std::string();

      GLuint programHandle = 0;

  }; // class Shader

} // namespace dg
