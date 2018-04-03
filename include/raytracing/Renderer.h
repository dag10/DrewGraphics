//
//  raytracer/Renderer.h
//

#pragma once

#include <Lights.h>
#include <raytracing/Rays.h>
#include <forward_list>
#include <glm/glm.hpp>
#include <memory>

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

      void ProcessSceneObjects();
      RayResult TraceRay(Ray ray);
      glm::vec3 ShadeRayResult(RayResult rayres);

      int numObjects = 0;
      std::forward_list<const TraceableModel*> objects;
      std::forward_list<const Light::ShaderData> lights;
      std::shared_ptr<Canvas> canvas;
      const Scene *scene;

  }; // class Renderer

} // namespace dg
