//
//  raytracer/Renderer.h
//

#pragma once

#include <raytracing/Rays.h>
#include <memory>
#include <forward_list>
#include <glm/glm.hpp>

namespace dg {

  class Texture;
  class Canvas;
  class Scene;
  class TraceableModel;

  class Renderer {

    public:

      Renderer(unsigned int width, unsigned int height, Scene *scene);

      void Render();

      std::shared_ptr<Texture> GetTexture() const;

    private:

      struct Pixel {
        uint8_t red   = 0;
        uint8_t green = 0;
        uint8_t blue  = 0;

        Pixel() = default;
        Pixel(uint8_t red, uint8_t green, uint8_t blue)
          : red(red), green(green), blue(blue) {}
        Pixel(glm::vec3 colors)
          : Pixel(colors.x * 255, colors.y * 255, colors.z * 255) {}
      };

      void ProcessSceneObjects();
      RayResult TraceRay(Ray ray);
      Pixel RenderPixel(RayResult rayres);

      int numObjects = 0;
      std::forward_list<const TraceableModel*> objects;
      std::shared_ptr<Canvas> canvas;
      const Scene *scene;

  }; // class Renderer

} // namespace dg
