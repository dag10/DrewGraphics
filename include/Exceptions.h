//
//  Exceptions.h
//

#pragma once

#include <stdexcept>
#include <string>
#include <openvr.h>
#include <glad/glad.h>

namespace dg {

class OpenVRError : public std::runtime_error {
  public:
    OpenVRError(vr::EVRInitError error) : std::runtime_error(
      VR_GetVRInitErrorAsSymbol(error)) { };
};

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

class FrameBufferException : public std::runtime_error {
  public:
    FrameBufferException(const GLuint status) : std::runtime_error(
        "FrameBuffer error. Status: " + std::to_string(status)) {}
};

class STBLoadError : public std::exception {
  public:
    STBLoadError(const std::string& path, const std::string& str) {
      reason = "Failed to load \"" + path + "\": " + str;
    }
    virtual const char* what() const noexcept {
      return reason.c_str();
    }
  private:
    std::string reason;
};

} // namespace dg

