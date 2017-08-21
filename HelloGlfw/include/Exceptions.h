//
//  Exceptions.h
//

#pragma once

#include <stdexcept>
#include <string>

namespace dg {

class ShaderCompileError : public std::runtime_error {
  public:
    ShaderCompileError(const std::string& path, const std::string& error)
        : std::runtime_error(
            "Unable to compile shader \"" + path + "\":\n" + error) { };
};

class ShaderLinkError : public std::runtime_error {
  public:
    ShaderLinkError(const std::string& error)
        : std::runtime_error("Unable to link shader:\n" + error) { };
};

class FileNotFoundException : public std::runtime_error {
  public:
    FileNotFoundException(const std::string& path)
        : std::runtime_error("Unable to open \"" + path + "\"") { };
};

class ResourceLoadException : public std::runtime_error {
  public:
    ResourceLoadException(const std::string& str) : std::runtime_error(str) {}
};

} // namespace dg

