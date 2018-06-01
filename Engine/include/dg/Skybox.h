//
//  Skybox.h
//
#pragma once

#include <openvr.h>
#include <memory>
#include "dg/Camera.h"
#include "dg/Cubemap.h"
#include "dg/Model.h"

namespace dg {

  class Skybox {

    public:

      bool enabled = true;

      static std::shared_ptr<Skybox> Create(std::shared_ptr<Texture> texture);
      static std::shared_ptr<Skybox> Create(std::shared_ptr<Cubemap> cubemap);

      void Draw(const Camera& camera);
      void Draw(const Camera& camera, vr::EVREye eye);

    private:

      virtual void Draw(const Camera& camera, glm::mat4x4 projection) = 0;

  }; // class Skybox

  class CubeMeshSkybox : public Skybox {

    friend class Skybox;

    private:

      Model model;

      CubeMeshSkybox(std::shared_ptr<Texture> texture);

      virtual void Draw(const Camera& camera, glm::mat4x4 projection);

  }; // class CubeMeshSkybox

  class CubemapSkybox : public Skybox {

    friend class Skybox;

    private:

      Model model;

      CubemapSkybox(std::shared_ptr<Cubemap> cubemap);

      virtual void Draw(const Camera& camera, glm::mat4x4 projection);

  }; // class CubemapSkybox

} // namespace dg
