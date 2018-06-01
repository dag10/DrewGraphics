//
//  Cubemap.h
//

#pragma once

#if defined(_OPENGL)
#include "dg/opengl/glad/glad.h"
#elif defined(_DIRECTX)
#error DirectXCubemap not implemented!
#endif

#include <memory>
#include <string>
#include "dg/Texture.h"

namespace dg {

  class OpenGLCubemap;
  class DirectXCubemap;
#if defined(_OPENGL)
  using Cubemap = OpenGLCubemap;
#elif defined(_DIRECTX)
  using Cubemap = DirectXCubemap;
#endif

  // Copy is disabled. This prevents us from leaking or redeleting
  // OpenGL/DirectX resources.
  class BaseCubemap {

    public:

      enum class Face { Right, Left, Top, Bottom, Back, Front, };

      static std::shared_ptr<Cubemap> FromPaths(const std::string &right,
                                                const std::string &left,
                                                const std::string &top,
                                                const std::string &bottom,
                                                const std::string &back,
                                                const std::string &front);
      static std::shared_ptr<Cubemap> Generate(TextureOptions options);

      BaseCubemap() = delete;

      virtual ~BaseCubemap() = default;

      BaseCubemap(BaseCubemap& other) = delete;
      BaseCubemap& operator=(BaseCubemap& other) = delete;

      virtual void UpdateData(Face face, const void *pixels,
                              bool genMipMap = true) = 0;
      virtual void GenerateMips() = 0;
      virtual void GenerateMips(Face face) = 0;

      const TextureOptions GetOptions() const;
      unsigned int GetWidth() const;
      unsigned int GetHeight() const;

    protected:

      BaseCubemap(TextureOptions options);

      void LoadImage(Face face, const std::string &filepath);

      virtual void GenerateImage(Face face, void *pixels = nullptr) = 0;

      TextureOptions options;

  }; // class BaseCubemap

#if defined(_OPENGL)

  class OpenGLCubemap : public BaseCubemap {
    friend class BaseCubemap;

    public:

      virtual ~OpenGLCubemap();

      virtual void UpdateData(Face face, const void *pixels,
                              bool genMipMap = true);
      virtual void GenerateMips();
      virtual void GenerateMips(Face face);

      GLuint GetHandle() const;

      void Bind() const;
      void Unbind() const;

    private:

      static GLenum FaceToGLTarget(Face face);

      OpenGLCubemap(TextureOptions options);

      void GenerateCubemap();
      virtual void GenerateImage(Face face, void *pixels);

      GLuint cubemapHandle = 0;

  }; // class OpenGLCubemap

#endif

} // namespace dg
