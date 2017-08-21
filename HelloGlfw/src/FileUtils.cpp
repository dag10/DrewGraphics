//
//  FileUtils.cpp
//

#include "FileUtils.h"
#include "Exceptions.h"

#include <fstream>
#include <sstream>

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

