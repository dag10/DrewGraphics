//
//  Image.h
//

#pragma once

#include <memory>
#include <string>

namespace dg {

  // Copy is disabled. This prevents us from leaking or redeleting resources.
  class Image {

    public:

      static std::shared_ptr<Image> FromPath(const std::string &path,
                                             bool flipVertically = false);

      Image(Image &other) = delete;
      Image &operator=(Image &other) = delete;

      virtual ~Image();

      const std::string &GetPath() const;
      unsigned char *GetPixels() const;
      unsigned int GetWidth() const;
      unsigned int GetHeight() const;

    protected:

      Image() = default;

      std::string path = "";
      unsigned char *pixels = nullptr;
      unsigned int width = 0;
      unsigned int height = 0;

  }; // class Image

} // namespace dg
