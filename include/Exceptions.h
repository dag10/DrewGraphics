//
//  Exceptions.h
//

#pragma once

#include <stdexcept>
#include <string>

namespace dg {

class FileNotFoundException : public std::runtime_error {
  public:
    FileNotFoundException(std::string path)
        : std::runtime_error("Unable to open \"" + path + "\"") { };
};

class ResourceLoadException : public std::runtime_error {
  public:
    ResourceLoadException(std::string str) : std::runtime_error(str) {}
};

} // namespace dg

