//
//  Shader.h
//

#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include <Texture.h>
#include <Transform.h>
#include <ShaderSource.h>
#include <glad/glad.h>

namespace dg {

  class OpenGLShader;

  // Copy is disabled. This prevents us from leaking or redeleting
  // OpenGL/DirectX resources.
  class Shader {

    public:

#if defined(_OPENGL)
      typedef OpenGLShader shader_class;
#endif

      static std::shared_ptr<Shader> FromFiles(
          const std::string& vertexPath, const std::string& fragmentPath);

      Shader() = default;
      virtual ~Shader() = default;

      Shader(Shader& other) = delete;
      Shader& operator=(Shader& other) = delete;

      virtual void Use() = 0;

      virtual void SetBool(const std::string& name, bool value) = 0;
      virtual void SetInt(const std::string& name, int value) = 0;
      virtual void SetFloat(const std::string& name, float value) = 0;
      virtual void SetVec2(const std::string& name, const glm::vec2& value) = 0;
      virtual void SetVec3(const std::string& name, const glm::vec3& value) = 0;
      virtual void SetVec4(const std::string& name, const glm::vec4& value) = 0;
      virtual void SetMat2(const std::string& name, const glm::mat2& mat) = 0;
      virtual void SetMat3(const std::string& name, const glm::mat3& mat) = 0;
      virtual void SetMat4(const std::string& name, const glm::mat4& mat) = 0;
      virtual void SetMat4(const std::string& name, const Transform& xf) = 0;
      virtual void SetTexture(
          unsigned int textureUnit, const std::string& name,
          Texture *texture) = 0;

    protected:

      std::string vertexPath = std::string();
      std::string fragmentPath = std::string();

  }; // class Shader

#if defined(_OPENGL)
  class OpenGLShader : public Shader {
    friend class Shader;

    public:
      static void SetVertexHead(const std::string& path);
      static void SetFragmentHead(const std::string& path);

      static void AddVertexSource(const std::string& path);
      static void AddFragmentSource(const std::string& path);

      OpenGLShader() = default;
      virtual ~OpenGLShader();

      OpenGLShader(OpenGLShader& other) = delete;
      OpenGLShader& operator=(OpenGLShader& other) = delete;

      virtual void Use();

      GLint GetUniformLocation(const std::string& name) const;
      GLint GetAttributeLocation(const std::string& name) const;

      virtual void SetBool(const std::string& name, bool value);
      virtual void SetInt(const std::string& name, int value);
      virtual void SetFloat(const std::string& name, float value);
      virtual void SetVec2(const std::string& name, const glm::vec2& value);
      virtual void SetVec3(const std::string& name, const glm::vec3& value);
      virtual void SetVec4(const std::string& name, const glm::vec4& value);
      virtual void SetMat2(const std::string& name, const glm::mat2& mat);
      virtual void SetMat3(const std::string& name, const glm::mat3& mat);
      virtual void SetMat4(const std::string& name, const glm::mat4& mat);
      virtual void SetMat4(const std::string& name, const Transform& xf);
      virtual void SetTexture(
          unsigned int textureUnit, const std::string& name, Texture *texture);

    private:

      static std::shared_ptr<OpenGLShader>FromFiles(
          const std::string& vertexPath, const std::string& fragmentPath);

      // Code to be included at top of shaders, includes global types.
      static std::string vertexHead;
      static std::string fragmentHead;

      // Code to be linked into all shaders, includes main() and utilities.
      static std::vector<dg::ShaderSource> vertexSources;
      static std::vector<dg::ShaderSource> fragmentSources;

      void CreateProgram();
      void CheckLinkErrors();

      GLuint programHandle = 0;

  }; // class OpenGLShader
#endif

} // namespace dg
