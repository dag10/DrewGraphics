//
//  opengl/ShaderSource.cpp
//

#include "dg/opengl/ShaderSource.h"
#include "dg/FileUtils.h"
#include "dg/Exceptions.h"

dg::ShaderSource dg::ShaderSource::FromFile(
    GLenum type, const std::string& path) {
  ShaderSource source;
  source.shaderType = type;
  source.path = path;
  source.CompileShader();
  return source;
}

dg::ShaderSource dg::ShaderSource::FromFileWithHeads(
    GLenum type, const std::string &path, std::vector<std::string> heads) {
  ShaderSource source;
  source.shaderType = type;
  source.path = path;
  source.heads = heads;
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
  swap(first.heads, second.heads);
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
  std::vector<const char *> codeStrings;
  for (const std::string &head : heads) {
    codeStrings.push_back(head.c_str());
  }
  codeStrings.push_back(code.c_str());
  glShaderSource(shaderHandle, (GLsizei)codeStrings.size(), codeStrings.data(),
                 NULL);
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
