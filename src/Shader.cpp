//
//  Shader.cpp
//

#include <glad/glad.h>
#include <Shader.h>
#include <Exceptions.h>
#include <FileUtils.h>
#include <ShaderSource.h>

#include <cassert>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

#pragma region Base Class

std::shared_ptr<dg::Shader> dg::Shader::FromFiles(
    const std::string& vertexPath, const std::string& fragmentPath) {
#if defined(_OPENGL)
  return std::static_pointer_cast<Shader>(
    OpenGLShader::FromFiles(vertexPath, fragmentPath));
#endif
}

#pragma endregion

#if defined(_OPENGL)
#pragma region OpenGL Shader

std::string dg::OpenGLShader::vertexHead = "";
std::string dg::OpenGLShader::fragmentHead = "";

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

void dg::OpenGLShader::SetVertexHead(const std::string& path) {
  vertexHead = dg::FileUtils::LoadFile(path);
}

void dg::OpenGLShader::SetFragmentHead(const std::string& path) {
  fragmentHead = dg::FileUtils::LoadFile(path);
}

void dg::OpenGLShader::AddVertexSource(const std::string& path) {
  dg::ShaderSource shader = vertexHead.empty()
      ? dg::ShaderSource::FromFile(GL_VERTEX_SHADER, path)
      : dg::ShaderSource::FromFileWithHead(GL_VERTEX_SHADER, path, vertexHead);
  vertexSources.push_back(std::move(shader));
}

void dg::OpenGLShader::AddFragmentSource(const std::string& path) {
  dg::ShaderSource shader = fragmentHead.empty()
      ? dg::ShaderSource::FromFile(GL_FRAGMENT_SHADER, path)
      : dg::ShaderSource::FromFileWithHead(GL_FRAGMENT_SHADER, path, fragmentHead);
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

  dg::ShaderSource vertexShader = vertexHead.empty()
      ? dg::ShaderSource::FromFile(GL_VERTEX_SHADER, vertexPath)
      : dg::ShaderSource::FromFileWithHead(
          GL_VERTEX_SHADER, vertexPath, vertexHead);
  dg::ShaderSource fragmentShader = fragmentHead.empty()
      ? dg::ShaderSource::FromFile(GL_FRAGMENT_SHADER, fragmentPath)
      : dg::ShaderSource::FromFileWithHead(
          GL_FRAGMENT_SHADER, fragmentPath, fragmentHead);

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

void dg::OpenGLShader::SetMat2(const std::string& name, const glm::mat2& mat) {
  glUniformMatrix2fv(
      GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void dg::OpenGLShader::SetMat3(const std::string& name, const glm::mat3& mat) {
  glUniformMatrix3fv(
      GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
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
    unsigned int textureUnit, const std::string& name, Texture *texture) {
  assert(texture != nullptr);

  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
  glUniform1i(GetUniformLocation(name), textureUnit);
}

#pragma endregion
#endif
