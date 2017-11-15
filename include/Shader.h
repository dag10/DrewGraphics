//
//  Shader.h
//

#pragma once

#include <vector>
#include <map>
#include <string>
#include <GLUT/glut.h>
#include <glm/glm.hpp>
#include <Texture.h>
#include <Transform.h>
#include <ShaderSource.h>

namespace dg {

  enum ShaderPropertyType {
    PROPERTY_NULL,

    PROPERTY_BOOL,
    PROPERTY_INT,
    PROPERTY_FLOAT,
    PROPERTY_VEC2,
    PROPERTY_VEC3,
    PROPERTY_VEC4,
    PROPERTY_MAT2X2,
    PROPERTY_MAT3X3,
    PROPERTY_MAT4X4,
    PROPERTY_TEXTURE,

    PROPERTY_MAX,
  };

  union ShaderPropertyValue {
    bool _bool;
    int _int;
    float _float;
    glm::vec2 _vec2;
    glm::vec3 _vec3;
    glm::vec4 _vec4;
    glm::mat2x2 _mat2x2;
    glm::mat3x3 _mat3x3;
    glm::mat4x4 _mat4x4;
  };

  struct ShaderProperty {
    ShaderPropertyType type = PROPERTY_NULL;
    ShaderPropertyValue value;
    Texture *texture; // Weak non-owning pointer.
  };

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL shader program resource.
  class Shader {

    public:
      static int MAX_VERTEX_TEXTURE_UNITS;
      static int MAX_GEOMETRY_TEXTURE_UNITS;
      static int MAX_FRAGMENT_TEXTURE_UNITS;
      static int MAX_COMBINED_TEXTURE_UNITS;

      static void Initialize();

      static Shader FromFiles(
          const std::string& vertexPath, const std::string& fragmentPath);

      static void SetVertexHead(const std::string& path);
      static void SetFragmentHead(const std::string& path);

      static void AddVertexSource(const std::string& path);
      static void AddFragmentSource(const std::string& path);

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

      void SetBool(const std::string& name, bool value);
      void SetInt(const std::string& name, int value);
      void SetFloat(const std::string& name, float value);
      void SetVec2(const std::string& name, const glm::vec2& value);
      void SetVec3(const std::string& name, const glm::vec3& value);
      void SetVec4(const std::string& name, const glm::vec4& value);
      void SetMat2(const std::string& name, const glm::mat2& mat);
      void SetMat3(const std::string& name, const glm::mat3& mat);
      void SetMat4(const std::string& name, const glm::mat4& mat);
      void SetMat4(const std::string& name, const Transform& xf);
      void SetTexture(
          unsigned int textureUnit, const std::string& name, Texture *texture);

    private:
      // Code to be included at top of shaders, includes global types.
      static std::string vertexHead;
      static std::string fragmentHead;

      // Code to be linked into all shaders, includes main() and utilities.
      static std::vector<dg::ShaderSource> vertexSources;
      static std::vector<dg::ShaderSource> fragmentSources;

      void CreateProgram();
      void CheckLinkErrors();

      std::string vertexPath = std::string();
      std::string fragmentPath = std::string();

      std::map<std::string, ShaderProperty> properties;

      GLuint programHandle = 0;

  }; // class Shader

} // namespace dg
