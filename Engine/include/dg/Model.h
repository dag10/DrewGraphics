//
//  Model.h
//
#pragma once

#include <memory>
#include <glm/mat4x4.hpp>

#include "dg/Material.h"
#include "dg/Mesh.h"
#include "dg/Scene.h"
#include "dg/SceneObject.h"

namespace dg {

  class Model : public SceneObject {

    public:

      struct DrawContext {
        glm::mat4x4 view = glm::mat4x4(1);
        glm::mat4x4 projection = glm::mat4x4(1);
        const glm::vec3 *cameraPos = nullptr;
        const Light::ShaderData (*lights)[Light::MAX_LIGHTS] = nullptr;
        std::shared_ptr<Texture> shadowMap = nullptr;
      };

      Model();

      Model(
          std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
          Transform transform);

      Model(Model& other);

      std::shared_ptr<Mesh> mesh = nullptr;
      std::shared_ptr<Material> material = nullptr;
      Scene::LayerMask layer = Scene::LayerMask::Default();

      void Draw(glm::mat4x4 view, glm::mat4x4 projection,
                std::shared_ptr<Material> material = nullptr) const;

      void Draw(const DrawContext &context,
                std::shared_ptr<Material> material = nullptr) const;

  }; // class Model

} // namespace dg
