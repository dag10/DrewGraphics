//
//  Model.h
//
#pragma once

#include <memory>
#include <glm/mat4x4.hpp>

#include "dg/SceneObject.h"
#include "dg/Material.h"
#include "dg/Mesh.h"

namespace dg {

  class Model : public SceneObject {

    public:

      Model();

      Model(
          std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
          Transform transform);

      Model(Model& other);

      std::shared_ptr<Mesh> mesh = nullptr;
      std::shared_ptr<Material> material = nullptr;

      void Draw(glm::mat4x4 view, glm::mat4x4 projection) const;

  }; // class Model

} // namespace dg
