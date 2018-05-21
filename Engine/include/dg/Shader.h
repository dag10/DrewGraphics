//
//  Shader.h
//

#pragma once

#if defined(_OPENGL)
#include "dg/opengl/glad/glad.h"

#include "dg/opengl/ShaderSource.h"
#elif defined(_DIRECTX)
#include "dg/directx/SimpleShader.h"
#endif

#include "dg/Texture.h"
#include "dg/Transform.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace dg {

  class OpenGLShader;
  class DirectXShader;

  // Copy is disabled. This prevents us from leaking or redeleting
  // OpenGL/DirectX resources.
  class Shader {

    public:

#if defined(_OPENGL)
      typedef OpenGLShader shader_class;
#elif defined(_DIRECTX)
      typedef DirectXShader shader_class;
#endif

      static std::shared_ptr<Shader> FromFiles(
          const std::string& vertexPath, const std::string& fragmentPath);
      static std::shared_ptr<Shader> FromFiles(const std::string& vertexPath,
                                               const std::string& geometryPath,
                                               const std::string& fragmentPath);

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
      virtual void SetMat4(const std::string& name, const glm::mat4& mat) = 0;
      virtual void SetMat4(const std::string& name, const Transform& xf) = 0;
      virtual void SetTexture(
          unsigned int textureUnit, const std::string& name,
          const Texture *texture) = 0;
      virtual void SetData(const std::string& name, void *data, size_t size) = 0;
      template <typename T>
      void SetData(const std::string& name, const T& data) {
        SetData(name, (void*)&data, sizeof(data));
      }

    protected:

      std::string vertexPath = std::string();
      std::string geometryPath = std::string();
      std::string fragmentPath = std::string();

  }; // class Shader

#if defined(_OPENGL)

  class OpenGLShader : public Shader {
    friend class Shader;

    public:

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
      virtual void SetMat4(const std::string& name, const glm::mat4& mat);
      virtual void SetMat4(const std::string& name, const Transform& xf);
      virtual void SetTexture(
          unsigned int textureUnit, const std::string& name,
          const Texture *texture);
      virtual void SetData(const std::string& name, void *data, size_t size);

    private:

      static std::shared_ptr<OpenGLShader>FromFiles(
          const std::string& vertexPath, const std::string& fragmentPath);
      static std::shared_ptr<OpenGLShader> FromFiles(
          const std::string& vertexPath, const std::string& geometryPath,
          const std::string& fragmentPath);

      void CreateProgram();
      void CheckLinkErrors();

      GLuint programHandle = 0;

  }; // class OpenGLShader

#elif defined(_DIRECTX)

  class DirectXShader : public Shader {
    friend class Shader;

    public:

      DirectXShader() = default;
      virtual ~DirectXShader() = default;

      DirectXShader(DirectXShader& other) = delete;
      DirectXShader& operator=(DirectXShader& other) = delete;

      virtual void Use();

      virtual void SetBool(const std::string& name, bool value);
      virtual void SetInt(const std::string& name, int value);
      virtual void SetFloat(const std::string& name, float value);
      virtual void SetVec2(const std::string& name, const glm::vec2& value);
      virtual void SetVec3(const std::string& name, const glm::vec3& value);
      virtual void SetVec4(const std::string& name, const glm::vec4& value);
      virtual void SetMat4(const std::string& name, const glm::mat4& mat);
      virtual void SetMat4(const std::string& name, const Transform& xf);
      virtual void SetTexture(
          unsigned int textureUnit, const std::string& name,
          const Texture *texture);
      virtual void SetData(const std::string& name, void *data, size_t size);

    private:

      static std::shared_ptr<DirectXShader>FromFiles(
          const std::string& vertexPath, const std::string& fragmentPath);

      std::shared_ptr<SimpleVertexShader> vertexShader = nullptr;
      std::shared_ptr<SimplePixelShader> pixelShader = nullptr;

  }; // class DirectXShader

#endif

} // namespace dg
