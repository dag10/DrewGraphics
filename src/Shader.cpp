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

int dg::Shader::MAX_VERTEX_TEXTURE_UNITS = 0;
int dg::Shader::MAX_GEOMETRY_TEXTURE_UNITS = 0;
int dg::Shader::MAX_FRAGMENT_TEXTURE_UNITS = 0;
int dg::Shader::MAX_COMBINED_TEXTURE_UNITS = 0;

void dg::Shader::Initialize() {
  int vertex, geometry, fragment, combined;
  glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &MAX_VERTEX_TEXTURE_UNITS);
  glGetIntegerv(
      GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &MAX_GEOMETRY_TEXTURE_UNITS);
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MAX_FRAGMENT_TEXTURE_UNITS);
  glGetIntegerv(
      GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_COMBINED_TEXTURE_UNITS);
}

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

void dg::Shader::SetBool(const std::string& name, bool value) {
  ShaderProperty prop;
  prop.type = PROPERTY_BOOL;
  prop.value._bool = value;
  glUniform1i(GetUniformLocation(name), (int)value);
  properties[name] = prop;
}

void dg::Shader::SetInt(const std::string& name, int value) {
  glUniform1i(GetUniformLocation(name), (int)value);
  ShaderProperty prop;
  prop.type = PROPERTY_INT;
  prop.value._int = value;
  properties[name] = prop;
}

void dg::Shader::SetFloat(const std::string& name, float value) {
  glUniform1f(GetUniformLocation(name), value);
  ShaderProperty prop;
  prop.type = PROPERTY_FLOAT;
  prop.value._float = value;
  properties[name] = prop;
}

void dg::Shader::SetVec2(
    const std::string& name, const glm::vec2& value) {
  glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
  ShaderProperty prop;
  prop.type = PROPERTY_VEC2;
  prop.value._vec2 = value;
  properties[name] = prop;
}

void dg::Shader::SetVec3(
    const std::string& name, const glm::vec3& value) {
  glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
  ShaderProperty prop;
  prop.type = PROPERTY_VEC3;
  prop.value._vec3 = value;
  properties[name] = prop;
}

void dg::Shader::SetVec4(const std::string& name, const glm::vec4& value) {
  glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
  ShaderProperty prop;
  prop.type = PROPERTY_VEC4;
  prop.value._vec4 = value;
  properties[name] = prop;
}

void dg::Shader::SetMat2(const std::string& name, const glm::mat2& mat) {
  glUniformMatrix2fv(
      GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
  ShaderProperty prop;
  prop.type = PROPERTY_MAT2X2;
  prop.value._mat2x2 = mat;
  properties[name] = prop;
}

void dg::Shader::SetMat3(const std::string& name, const glm::mat3& mat) {
  glUniformMatrix3fv(
      GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
  ShaderProperty prop;
  prop.type = PROPERTY_MAT3X3;
  prop.value._mat3x3 = mat;
  properties[name] = prop;
}

void dg::Shader::SetMat4(const std::string& name, const glm::mat4& mat) {
  glUniformMatrix4fv(
      GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
  ShaderProperty prop;
  prop.type = PROPERTY_MAT4X4;
  prop.value._mat4x4 = mat;
  properties[name] = prop;
}

void dg::Shader::SetMat4(
    const std::string& name, const dg::Transform& xf) {
  glm::mat4x4 mat = xf.ToMat4();
  SetMat4(name, mat);
  ShaderProperty prop;
  prop.type = PROPERTY_MAT4X4;
  prop.value._mat4x4 = mat;
  properties[name] = prop;
}

void dg::Shader::SetTexture(
    unsigned int textureUnit, const std::string& name, Texture *texture) {
  assert(texture != nullptr);

  if (properties.find(name) != properties.end() &&
      properties[name].texture == texture) {
    return;
  }

  ShaderProperty prop;
  prop.type = PROPERTY_TEXTURE;
  prop.texture = texture;
  properties[name] = prop;

  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
  glUniform1i(GetUniformLocation(name), textureUnit);
}

