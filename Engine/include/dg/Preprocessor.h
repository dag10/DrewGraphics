//
//  Preprocessor.h
//

#pragma once

#include <cassert>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "dg/Exceptions.h"

namespace dg {

  class Preprocessor {

    public:

      static std::shared_ptr<Preprocessor> ForFile(const std::string &filename);

      const std::string &GetProcessedContent();
      inline const std::string &GetFilename() const {
        return filename;
      }

    private:

      struct File {
        std::string filename;
        std::vector<std::string> lines;
        std::string processedContent;
        int id;
        bool processed = false;
      };

      class UnterminatedQuoteError : public EngineError {
        public:
          UnterminatedQuoteError(const File &file, int line)
              : EngineError("Unterminated quote on #include in file " +
                            file.filename + ":" + std::to_string(line)) {}
      };

      class EmptyError : public EngineError {
        public:
          EmptyError(const File &file, int line)
              : EngineError("Empty #include for file " +
                            file.filename + ":" + std::to_string(line)) {}
      };

      class CycleError : public EngineError {
        public:
          CycleError(const File &file, int line)
              : EngineError("Include cycle detected for file " +
                            file.filename + ":" + std::to_string(line)) {}
      };

      Preprocessor() = default;

      File &GetFile(const std::string &filename);
      File &GetProcessedFile(File &file);

      std::string filename;
      std::unordered_map<std::string, File> files;
      std::set<std::string> currentFilenames;
      int nextFileID = 0;

  }; // class Preprocessor

} // namespace dg
