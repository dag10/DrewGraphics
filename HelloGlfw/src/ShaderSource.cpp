//
//  ShaderSource.cpp
//

#include <ShaderSource.h>
#include <FileUtils.h>
#include <Exceptions.h>

dg::ShaderSource dg::ShaderSource::FromFile(
    GLenum type, const std::string& path) {
  ShaderSource source;
  source.shaderType = type;
  source.path = path;
  source.CompileShader();
  return source;
}

dg::ShaderSource dg::ShaderSource::FromFileWithHead(
    GLenum type, const std::string& path, const std::string& headCode) {
  ShaderSource source;
  source.shaderType = type;
  source.path = path;
  source.headCode = headCode.c_str();
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
  swap(first.headCode, second.headCode);
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
  shaderHandle = glCreateShader(shaderType);
  if (headCode == nullptr) {
    const char *codeString = code.c_str();
    glShaderSource(shaderHandle, 1, &codeString, NULL);
  } else {
    const char *codeStrings[2];
    codeStrings[0] = headCode;
    codeStrings[1] = code.c_str();
    glShaderSource(shaderHandle, 2, codeStrings, NULL);
  }
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

