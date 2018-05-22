//
//  opengl/ShaderSource.cpp
//

#include "dg/opengl/ShaderSource.h"
#include "dg/FileUtils.h"
#include "dg/Exceptions.h"

std::shared_ptr<dg::ShaderSource> dg::ShaderSource::FromFile(
    GLenum type, const std::string& path) {
  auto source = std::shared_ptr<ShaderSource>(new ShaderSource());
  source->shaderType = type;
  source->file = Preprocessor::ForFile(path);
  source->CompileShader();
  return source;
}

dg::ShaderSource::~ShaderSource() {
  if (shaderHandle != 0) {
    glDeleteShader(shaderHandle);
    shaderHandle = 0;
  }
}

void dg::ShaderSource::CompileShader() {
  assert(shaderHandle == 0);
  shaderHandle = glCreateShader(shaderType);
  const char *code = GetContent().c_str();
  glShaderSource(shaderHandle, 1, &code, NULL);
  glCompileShader(shaderHandle);
  CheckCompileErrors();
}

void dg::ShaderSource::CheckCompileErrors() {
  GLint success;
  GLchar log[1024];
  glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderHandle, sizeof(log), nullptr, log);
    throw ShaderCompileError(GetPath(), std::string(log));
  }
}
