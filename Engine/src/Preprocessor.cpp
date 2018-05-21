//
//  Preprocessor.cpp
//

#include "dg/Preprocessor.h"
#include <fstream>
#include <sstream>
#include <string>
#include "dg/FileUtils.h"

std::shared_ptr<dg::Preprocessor> dg::Preprocessor::ForFile(
    const std::string &filename) {
  auto pp = std::shared_ptr<Preprocessor>(new Preprocessor());
  pp->filename = filename;
  return pp;
}

const std::string &dg::Preprocessor::GetProcessedContent() {
  return GetProcessedFile(GetFile(filename)).processedContent;
}

dg::Preprocessor::File &dg::Preprocessor::GetFile(const std::string &filename) {
  if (files.find(filename) == files.end()) {
    files[filename] = {};
    files[filename].filename = filename;
    files[filename].id = nextFileID;
    nextFileID++;

    std::ifstream file;
    file.exceptions(std::ifstream::badbit);
    file.open(filename);
    if (file.fail()) {
      throw dg::FileNotFoundException(filename);
    }

    std::string line;
    while (std::getline(file, line)) {
      files[filename].lines.push_back(line);
    }
    file.close();
  }

  return files.at(filename);
}

dg::Preprocessor::File &dg::Preprocessor::GetProcessedFile(File &file) {
  currentFilenames.insert(file.filename);

  if (file.processed) {
    return file;
  }

  std::stringstream content;

  int lineNumber = 0;
  bool inComment = false;
  for (std::string &line : file.lines) {
    lineNumber++;

    // FIXME: This is really sloppy naive parsing for comments, block comments,
    //        and includes. I'm implementing this to solve a specific problem,
    //        but it should ideally be tokenized and parsed accurately.

    if (inComment) {
      if (line.find("*/") != std::string::npos) {
        inComment = false;
      }

      // We're in a block comment, so ignore this line.
      continue;
    }

    if (line.find("/*") != std::string::npos) {
      inComment = true;
      continue;
    }

    const std::string lineCommentPrefix = "//";
    const std::string includePatternPrefix = "#include \"";
    const std::string includePatternSuffix = "\"";

    size_t includeQuoteStart = line.find(includePatternPrefix);
    if (includeQuoteStart == std::string::npos) {
      content << line << std::endl;
      continue;
    }
    includeQuoteStart += includePatternPrefix.length();

    // See if a line comment has started before the #include.
    size_t lineCommentStart = line.find(lineCommentPrefix);
    if (lineCommentStart != std::string::npos &&
        lineCommentStart <= includeQuoteStart) {
      content << line << std::endl;
      continue;
    }

    size_t includeQuoteEnd =
        line.find(includePatternSuffix, includeQuoteStart);
    if (includeQuoteStart == std::string::npos) {
      throw UnterminatedQuoteError(file, lineNumber);
    }

    if (includeQuoteEnd <= includeQuoteStart) {
      throw EmptyError(file, lineNumber);
    };

    std::string includeFilename =
        line.substr(includeQuoteStart, includeQuoteEnd - includeQuoteStart);

    std::string includeFilePath = FileUtils::FlattenPath(
        FileUtils::DirectoryPathOfFilePath(file.filename) + "/" +
        includeFilename);

    if (currentFilenames.count(includeFilePath) > 0) {
      throw CycleError(file, lineNumber);
    }

    auto &includeFile = GetProcessedFile(GetFile(includeFilePath));
    content << "#line 1 " << includeFile.id << std::endl;
    content << includeFile.processedContent;
    content << "#line " << (lineNumber + 1) << " " << file.id << std::endl;
  }

  file.processedContent = content.str();
  currentFilenames.erase(file.filename);
  return file;
}
