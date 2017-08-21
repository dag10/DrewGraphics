//
//  ShaderSource.cpp
//

#include "ShaderSource.h"
#include "FileUtils.h"
#include "Exceptions.h"

dg::ShaderSource dg::ShaderSource::FromFile(
    GLenum type, const std::string& path) {
  ShaderSource source;
  source.shaderType = type;
  source.path = path;
  source.CompileShader();
  return source;
}

dg::ShaderSource::ShaderSource(dg::ShaderSource&& other) {
  *this = std::move(other);
}

dg::ShaderSource::~ShaderSource() {
  if (shaderHandle != 0) {
    glDeleteShader(shaderHandle);
    shaderHandle = 0;
  }
}

dg::ShaderSource& dg::ShaderSource::operator=(dg::ShaderSource&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(ShaderSource& first, ShaderSource& second) {
  using std::swap;
  swap(first.path, second.path);
  swap(first.shaderType, second.shaderType);
  swap(first.shaderHandle, second.shaderHandle);
}

GLuint dg::ShaderSource::GetHandle() const {
  return shaderHandle;
}

void dg::ShaderSource::CompileShader() {
  assert(shaderHandle == 0);

  std::string code = dg::FileUtils::LoadFile(path);
  const char *codeString = code.c_str();
  shaderHandle = glCreateShader(shaderType);
  glShaderSource(shaderHandle, 1, &codeString, NULL);
  glCompileShader(shaderHandle);
  CheckCompileErrors();
}

void dg::ShaderSource::CheckCompileErrors() {
  GLint success;
  GLchar log[1024];
  glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderHandle, sizeof(log), nullptr, log);
    throw dg::ShaderCompileError(path, std::string(log));
  }
}

