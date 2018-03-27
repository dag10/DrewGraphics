//
//  Skybox.h
//
#pragma once

#include <openvr.h>
#include <memory>
#include "dg/Camera.h"
#include "dg/Model.h"

namespace dg {

  class Skybox {

    public:

      bool enabled = true;

      static std::shared_ptr<Skybox> Create(std::shared_ptr<Texture> texture);

      void SetTexture(std::shared_ptr<Texture> texture);

      void Draw(const Camera& camera);
      void Draw(const Camera& camera, vr::EVREye eye);

    private:

      Model model;

      Skybox(std::shared_ptr<Texture> texture);

      void Draw(const Camera& camera, glm::mat4x4 projection);

  }; // class Skybox

} // namespace dg
