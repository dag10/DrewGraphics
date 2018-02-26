//
//  Shader.h
//

#pragma once

#include "dg/directx/SimpleShader.h"

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

      typedef DirectXShader shader_class;

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
      std::string fragmentPath = std::string();

  }; // class Shader


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


} // namespace dg
