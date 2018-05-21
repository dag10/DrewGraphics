//
//  FileUtils.h
//

#pragma once

#include <string>

namespace dg {

  class FileUtils {

    public:

      static std::string LoadFile(const std::string &path);
      static std::string DirectoryPathOfFilePath(const std::string &filename);
      static std::string FlattenPath(const std::string &path);

  }; // class FileUtils

} // namespace dg
