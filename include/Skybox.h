//
//  Skybox.h
//
#pragma once

#include <memory>
#include "Texture.h"
#include "Camera.h"
#include "Window.h"
#include "materials/StandardMaterial.h"
#include <openvr.h>

namespace dg {

  class Skybox {

    public:

      bool enabled = true;
      StandardMaterial material;

      Skybox() = default;
      Skybox(Skybox& other);
      Skybox(Skybox&& other);
      Skybox& operator=(Skybox& other);
      Skybox& operator=(Skybox&& other);
      Skybox(std::shared_ptr<Texture> texture);
      friend void swap(Skybox& first, Skybox& second); // nothrow

      void Draw(const Camera& camera, const Window& window);
      void Draw(const Camera& camera, vr::EVREye eye);

    private:

      void Draw(const Camera& camera,  glm::mat4x4 projection);

  }; // class Skybox

} // namespace dg
