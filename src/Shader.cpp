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

#if defined(_DIRECTX)
// For the DirectX Math library
using namespace DirectX;
#endif

#pragma region Base Class

std::shared_ptr<dg::Shader> dg::Shader::FromFiles(
    const std::string& vertexPath, const std::string& fragmentPath) {
#if defined(_OPENGL)
  return std::static_pointer_cast<Shader>(
    OpenGLShader::FromFiles(vertexPath, fragmentPath));
#elif defined(_DIRECTX)
  return std::static_pointer_cast<Shader>(
    DirectXShader::FromFiles(vertexPath, fragmentPath));
#endif
}

#pragma endregion

#if defined(_OPENGL)
#pragma region OpenGL Shader

std::vector<std::string> dg::OpenGLShader::vertexHeads =
    std::vector<std::string>();
std::vector<std::string> dg::OpenGLShader::fragmentHeads =
    std::vector<std::string>();

std::vector<dg::ShaderSource> dg::OpenGLShader::vertexSources = \
    std::vector<dg::ShaderSource>();
std::vector<dg::ShaderSource> dg::OpenGLShader::fragmentSources = \
    std::vector<dg::ShaderSource>();

std::shared_ptr<dg::OpenGLShader> dg::OpenGLShader::FromFiles(
    const std::string& vertexPath, const std::string& fragmentPath) {
  auto shader = std::make_shared<OpenGLShader>();
  shader->vertexPath = vertexPath;
  shader->fragmentPath = fragmentPath;
  shader->CreateProgram();
  return shader;
}

void dg::OpenGLShader::AddVertexHead(const std::string& path) {
  vertexHeads.push_back(dg::FileUtils::LoadFile(path));
}

void dg::OpenGLShader::AddFragmentHead(const std::string& path) {
  fragmentHeads.push_back(dg::FileUtils::LoadFile(path));
}

void dg::OpenGLShader::AddVertexSource(const std::string& path) {
  dg::ShaderSource shader =
      vertexHeads.empty() ? dg::ShaderSource::FromFile(GL_VERTEX_SHADER, path)
                          : dg::ShaderSource::FromFileWithHeads(
                                GL_VERTEX_SHADER, path, vertexHeads);
  vertexSources.push_back(std::move(shader));
}

void dg::OpenGLShader::AddFragmentSource(const std::string& path) {
  dg::ShaderSource shader =
      fragmentHeads.empty()
          ? dg::ShaderSource::FromFile(GL_FRAGMENT_SHADER, path)
          : dg::ShaderSource::FromFileWithHeads(GL_FRAGMENT_SHADER, path,
                                                fragmentHeads);
  fragmentSources.push_back(std::move(shader));
}

dg::OpenGLShader::~OpenGLShader() {
  if (programHandle != 0) {
    glDeleteProgram(programHandle);
    programHandle = 0;
  }
}

void dg::OpenGLShader::CreateProgram() {
  assert(programHandle == 0);

  dg::ShaderSource vertexShader =
      vertexHeads.empty()
          ? dg::ShaderSource::FromFile(GL_VERTEX_SHADER, vertexPath)
          : dg::ShaderSource::FromFileWithHeads(GL_VERTEX_SHADER, vertexPath,
                                                vertexHeads);
  dg::ShaderSource fragmentShader =
      fragmentHeads.empty()
          ? dg::ShaderSource::FromFile(GL_FRAGMENT_SHADER, fragmentPath)
          : dg::ShaderSource::FromFileWithHeads(GL_FRAGMENT_SHADER,
                                                fragmentPath, fragmentHeads);

  programHandle = glCreateProgram();
  glAttachShader(programHandle, vertexShader.GetHandle());
  glAttachShader(programHandle, fragmentShader.GetHandle());
  for (
      auto shader = vertexSources.begin();
      shader != vertexSources.end();
      shader++) {
    glAttachShader(programHandle, shader->GetHandle());
  }
  for (
      auto shader = fragmentSources.begin();
      shader != fragmentSources.end();
      shader++) {
    glAttachShader(programHandle, shader->GetHandle());
  }
  glLinkProgram(programHandle);
  CheckLinkErrors();
}

void dg::OpenGLShader::CheckLinkErrors() {
  GLint success;
  GLchar log[1024];
  glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(programHandle, sizeof(log), nullptr, log);
    throw dg::ShaderLinkError(std::string(log));
  }
}

void dg::OpenGLShader::Use() {
  glUseProgram(programHandle);
}

GLint dg::OpenGLShader::GetUniformLocation(const std::string& name) const {
  return glGetUniformLocation(programHandle, name.c_str());
}

GLint dg::OpenGLShader::GetAttributeLocation(const std::string& name) const {
  return glGetAttribLocation(programHandle, name.c_str());
}

void dg::OpenGLShader::SetBool(const std::string& name, bool value) {
  glUniform1i(GetUniformLocation(name), (int)value);
}

void dg::OpenGLShader::SetInt(const std::string& name, int value) {
  glUniform1i(GetUniformLocation(name), (int)value);
}

void dg::OpenGLShader::SetFloat(const std::string& name, float value) {
  glUniform1f(GetUniformLocation(name), value);
}

void dg::OpenGLShader::SetVec2(
    const std::string& name, const glm::vec2& value) {
  glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void dg::OpenGLShader::SetVec3(
    const std::string& name, const glm::vec3& value) {
  glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void dg::OpenGLShader::SetVec4(const std::string& name, const glm::vec4& value) {
  glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void dg::OpenGLShader::SetMat4(const std::string& name, const glm::mat4& mat) {
  glUniformMatrix4fv(
      GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void dg::OpenGLShader::SetMat4(
    const std::string& name, const dg::Transform& xf) {
  glm::mat4x4 mat = xf.ToMat4();
  SetMat4(name, mat);
}

void dg::OpenGLShader::SetTexture(
    unsigned int textureUnit, const std::string& name, const Texture *texture) {
  assert(texture != nullptr);

  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
  glUniform1i(GetUniformLocation(name), textureUnit);
}

void dg::OpenGLShader::SetData(
    const std::string& name, void *data, size_t size) {
  throw std::runtime_error("OpenGLShader::SetData() not implemented.");
}

#pragma endregion
#endif
#pragma region DirectX Shader
#if defined(_DIRECTX)

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
#endif
