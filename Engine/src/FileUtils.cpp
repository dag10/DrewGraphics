//
//  FileUtils.cpp
//

#include "dg/FileUtils.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "dg/Exceptions.h"

std::string dg::FileUtils::LoadFile(const std::string& path) {
  std::ifstream file;
  std::stringstream fileStream;

  file.exceptions(std::ifstream::badbit);
  file.open(path);
  if (file.fail()) {
    throw dg::FileNotFoundException(path);
  }
  fileStream << file.rdbuf();
  file.close();

  return fileStream.str();
}

std::string dg::FileUtils::DirectoryPathOfFilePath(
    const std::string &filename) {
  size_t lastSlashPos = filename.find_last_of("/\\");

  if (lastSlashPos != std::string::npos) {
    return filename.substr(0, lastSlashPos);
  }

  return "";
}

std::string dg::FileUtils::FlattenPath(const std::string &path) {
  if (path.length() <= 1) {
    return path;
  }

  std::vector<std::string> components;
  const std::string anyslash = "/\\";
  std::string slash = "";

  size_t firstSlash = path.find_first_of(anyslash);
  if (firstSlash != std::string::npos) {
    slash = path.substr(firstSlash, 1);
  }

  bool leadingSlash = (path.find_first_of(anyslash) == 0);
  bool trailingSlash = (path.find_last_of(anyslash) == path.length() - 1);

  size_t offset = leadingSlash ? 1 : 0;
  size_t length = path.length();
  while (offset < length) {
    size_t nextSlashPos = path.find_first_of(anyslash, offset + 1);
    if (nextSlashPos == std::string::npos) {
      nextSlashPos = length;
    }
    components.push_back(path.substr(offset, nextSlashPos - offset));
    offset = nextSlashPos + 1;
  }

  bool changed = false;
  do {
    changed = false;
    for (int i = 1; i < components.size(); i++) {
      if (components[i] == ".") {
        components.erase(components.begin() + i);
        i--;
        changed = true;
      } else if (components[i] == ".." && components[i - 1] != "..") {
        components.erase(components.begin() + i);
        components.erase(components.begin() + i - 1);
        changed = true;
        break;
      }
    }
  } while (changed);

  std::stringstream newpath;
  bool first = true;
  if (leadingSlash) {
    newpath << slash;
  }
  for (std::string &component : components) {
    if (!first) {
      newpath << slash;
    }
    newpath << component;
    first = false;
  }
  if (trailingSlash) {
    newpath << slash;
  }
  return newpath.str();
}
