//
//  raytracer/Renderer.h
//

#pragma once

#include <memory>

namespace dg {

  class Texture;
  class Canvas;
  class Scene;

  class Renderer {

    public:

      Renderer(unsigned int width, unsigned int height, Scene *scene);

      void Render();

      std::shared_ptr<Texture> GetTexture() const;

    private:

      std::shared_ptr<Canvas> canvas;
      Scene *scene;

  }; // class Renderer

} // namespace dg
