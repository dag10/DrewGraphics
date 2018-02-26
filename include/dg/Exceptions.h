//
//  Exceptions.h
//

#pragma once

#include <stdexcept>
#include <string>

namespace dg {

class EngineError : public std::runtime_error {
  public:
    EngineError(const std::string& msg) : std::runtime_error(msg) {}
};

class ShaderCompileError : public EngineError {
  public:
    ShaderCompileError(const std::string& path, const std::string& error)
        : EngineError(
            "Unable to compile shader \"" + path + "\":\n" + error) { };
};

class ShaderLoadException : public EngineError {
public:
  ShaderLoadException(const std::string& path)
    : EngineError("Failed to load shader \"" + path + "\"") {}
};

class ShaderLinkError : public EngineError {
  public:
    ShaderLinkError(const std::string& error)
        : EngineError("Unable to link shader:\n" + error) { };
};

class FileNotFoundException : public EngineError {
  public:
    FileNotFoundException(const std::string& path)
        : EngineError("Unable to open \"" + path + "\"") { };
};

class ResourceLoadException : public EngineError {
  public:
    ResourceLoadException(const std::string& str) : EngineError(str) {}
};

class STBLoadError : public EngineError {
  public:
    STBLoadError(const std::string& path, const std::string& str)
      : EngineError("Failed to load \"" + path + "\": " + str) {}
};

} // namespace dg

