//
//  raytracer/Renderer.h
//

#pragma once

#include <memory>
#include <glm/glm.hpp>

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

      struct Ray {
        glm::vec3 origin;
        glm::vec3 direction;

        Ray(glm::vec3 origin, glm::vec3 direction)
          : origin(origin), direction(direction) {}
      };

      struct RayResult {
        Ray ray;
        float distance;

        RayResult(Ray ray, float distance)
          : ray(ray), distance(distance) {}
      };

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

      RayResult TraceRay(Ray ray);
      Pixel RenderPixel(RayResult rayres);

      std::shared_ptr<Canvas> canvas;
      const Scene *scene;

  }; // class Renderer

} // namespace dg
