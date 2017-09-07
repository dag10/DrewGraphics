//
//  Shader.cpp
//

#include "Shader.h"
#include "Exceptions.h"
#include "FileUtils.h"
#include "ShaderSource.h"

#include <cassert>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

std::string dg::Shader::vertexHead = "";
std::string dg::Shader::fragmentHead = "";

std::vector<dg::ShaderSource> dg::Shader::vertexSources = \
    std::vector<dg::ShaderSource>();
std::vector<dg::ShaderSource> dg::Shader::fragmentSources = \
    std::vector<dg::ShaderSource>();

dg::Shader dg::Shader::FromFiles(
    const std::string& vertexPath, const std::string& fragmentPath) {
  Shader shader;
  shader.vertexPath = vertexPath;
  shader.fragmentPath = fragmentPath;
  shader.CreateProgram();
  return shader;
}

void dg::Shader::SetVertexHead(const std::string& path) {
  vertexHead = dg::FileUtils::LoadFile(path);
}

void dg::Shader::SetFragmentHead(const std::string& path) {
  fragmentHead = dg::FileUtils::LoadFile(path);
}

void dg::Shader::AddVertexSource(const std::string& path) {
  dg::ShaderSource shader = vertexHead.empty()
      ? dg::ShaderSource::FromFile(GL_VERTEX_SHADER, path)
      : dg::ShaderSource::FromFileWithHead(GL_VERTEX_SHADER, path, vertexHead);
  vertexSources.push_back(std::move(shader));
}

void dg::Shader::AddFragmentSource(const std::string& path) {
  dg::ShaderSource shader = fragmentHead.empty()
      ? dg::ShaderSource::FromFile(GL_FRAGMENT_SHADER, path)
      : dg::ShaderSource::FromFileWithHead(GL_FRAGMENT_SHADER, path, fragmentHead);
  fragmentSources.push_back(std::move(shader));
}

dg::Shader::Shader(dg::Shader&& other) {
  *this = std::move(other);
}

dg::Shader::~Shader() {
  if (programHandle != 0) {
    glDeleteProgram(programHandle);
    programHandle = 0;
  }
}

dg::Shader& dg::Shader::operator=(dg::Shader&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(Shader& first, Shader& second) {
  using std::swap;
  swap(first.vertexPath, second.vertexPath);
  swap(first.fragmentPath, second.fragmentPath);
  swap(first.programHandle, second.programHandle);
}

void dg::Shader::CreateProgram() {
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

void dg::Shader::CheckLinkErrors() {
  GLint success;
  GLchar log[1024];
  glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(programHandle, sizeof(log), nullptr, log);
    throw dg::ShaderLinkError(std::string(log));
  }
}

void dg::Shader::Use() {
  glUseProgram(programHandle);
}

GLint dg::Shader::GetUniformLocation(const std::string& name) const {
  return glGetUniformLocation(programHandle, name.c_str());
}

GLint dg::Shader::GetAttributeLocation(const std::string& name) const {
  return glGetAttribLocation(programHandle, name.c_str());
}

void dg::Shader::SetBool(const std::string& name, bool value) const {         
  glUniform1i(GetUniformLocation(name), (int)value); 
}

void dg::Shader::SetInt(const std::string& name, int value) const { 
  glUniform1i(GetUniformLocation(name), (int)value); 
}

void dg::Shader::SetFloat(const std::string& name, float value) const { 
  glUniform1f(GetUniformLocation(name), value); 
}

void dg::Shader::SetVec2(
    const std::string& name, const glm::vec2& value) const { 
  glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void dg::Shader::SetVec2(const std::string& name, float x, float y) const { 
  glUniform2f(GetUniformLocation(name), x, y); 
}

void dg::Shader::SetVec3(
    const std::string& name, const glm::vec3& value) const { 
  glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void dg::Shader::SetVec3(
    const std::string& name, float x, float y, float z) const { 
  glUniform3f(GetUniformLocation(name), x, y, z); 
}

void dg::Shader::SetVec4(const std::string& name, const glm::vec4& value) const { 
  glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void dg::Shader::SetVec4(
    const std::string& name, float x, float y, float z, float w) { 
  glUniform4f(GetUniformLocation(name), x, y, z, w); 
}

void dg::Shader::SetMat2(const std::string& name, const glm::mat2& mat) const {
  glUniformMatrix2fv(
      GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void dg::Shader::SetMat3(const std::string& name, const glm::mat3& mat) const {
  glUniformMatrix3fv(
      GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void dg::Shader::SetMat4(const std::string& name, const glm::mat4& mat) const {
  glUniformMatrix4fv(
      GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void dg::Shader::SetTexture(
    unsigned int textureUnit, const std::string& name,
    const dg::Texture& texture) const {
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, texture.GetHandle());
  SetInt(name, textureUnit);
}

