//
//  Shader.cpp
//

#include "dg/Shader.h"
#include <cassert>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "dg/Exceptions.h"
#include "dg/FileUtils.h"
#include "dg/Utils.h"

// For the DirectX Math library
using namespace DirectX;

#pragma region Base Class

std::shared_ptr<dg::Shader> dg::Shader::FromFiles(
    const std::string& vertexPath, const std::string& fragmentPath) {
  return std::static_pointer_cast<Shader>(
    DirectXShader::FromFiles(vertexPath, fragmentPath));
}

#pragma endregion

#pragma region DirectX Shader

std::shared_ptr<dg::DirectXShader> dg::DirectXShader::FromFiles(
    const std::string& vertexPath, const std::string& fragmentPath) {
  auto shader = std::make_shared<DirectXShader>();
  shader->vertexPath = vertexPath;
  shader->fragmentPath = fragmentPath;

  shader->vertexShader = std::make_shared<SimpleVertexShader>();
  auto wVertexPath = ToLPCWSTR(vertexPath);
  if (!shader->vertexShader->LoadShaderFile(wVertexPath.data())) {
    throw ShaderLoadException(vertexPath);
  }

  shader->pixelShader = std::make_shared<SimplePixelShader>();
  auto wPixelPath = ToLPCWSTR(fragmentPath);
  if (!shader->pixelShader->LoadShaderFile(wPixelPath.data())) {
    throw ShaderLoadException(fragmentPath);
  }

  return shader;
}

void dg::DirectXShader::Use() {
  vertexShader->SetShader();
  pixelShader->SetShader();
  vertexShader->CopyAllBufferData();
  pixelShader->CopyAllBufferData();
}

void dg::DirectXShader::SetBool(const std::string& name, bool value) {
  vertexShader->SetInt(name, value);
  pixelShader->SetInt(name, value);
}

void dg::DirectXShader::SetInt(const std::string& name, int value) {
  vertexShader->SetInt(name, value);
  pixelShader->SetInt(name, value);
}

void dg::DirectXShader::SetFloat(const std::string& name, float value) {
  vertexShader->SetFloat(name, value);
  pixelShader->SetFloat(name, value);
}

void dg::DirectXShader::SetVec2(
  const std::string& name, const glm::vec2& value) {
  vertexShader->SetFloat2(name, (XMFLOAT2&)value);
  pixelShader->SetFloat2(name, (XMFLOAT2&)value);
}

void dg::DirectXShader::SetVec3(
  const std::string& name, const glm::vec3& value) {
  vertexShader->SetFloat3(name, (XMFLOAT3&)value);
  pixelShader->SetFloat3(name, (XMFLOAT3&)value);
}

void dg::DirectXShader::SetVec4(const std::string& name, const glm::vec4& value) {
  vertexShader->SetFloat4(name, (XMFLOAT4&)value);
  pixelShader->SetFloat4(name, (XMFLOAT4&)value);
}

void dg::DirectXShader::SetMat4(const std::string& name, const glm::mat4& mat) {
  vertexShader->SetMatrix4x4(name, glm::value_ptr(mat));
  pixelShader->SetMatrix4x4(name, glm::value_ptr(mat));
}

void dg::DirectXShader::SetMat4(
  const std::string& name, const dg::Transform& xf) {
  SetMat4(name, xf.ToMat4());
}

void dg::DirectXShader::SetTexture(
    unsigned int textureUnit, const std::string& name,
    const Texture *texture) {
  assert(texture != nullptr);
  bool a = pixelShader->SetSamplerState(name + "Sampler", texture->GetSamplerState());
  bool b = pixelShader->SetShaderResourceView(name,
                                     texture->GetShaderResourceView());
}

void dg::DirectXShader::SetData(
    const std::string& name, void *data, size_t size) {
  vertexShader->SetData(name, data, size);
  pixelShader->SetData(name, data, size);
}

#pragma endregion
