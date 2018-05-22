//
//  FileUtils.h
//

#pragma once

#include <string>
#include <vector>

namespace dg {

  class FileUtils {

    public:

      static std::string LoadFile(const std::string &path);
      static std::vector<std::string> LoadFileLines(const std::string &path);
      static std::string DirectoryPathOfFilePath(const std::string &filename);
      static std::string FlattenPath(const std::string &path);

  }; // class FileUtils

} // namespace dg
