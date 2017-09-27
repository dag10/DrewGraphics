//
//  Model.h
//
#pragma once

#include <memory>
#include <glm/mat4x4.hpp>

#include "Transform.h"
#include "Material.h"
#include "Mesh.h"

namespace dg {

  class Model {

    public:

      Model() = default;

      Model(
          std::shared_ptr<Mesh> mesh, Material material, Transform transform);

      Model(Model& other);
      Model(Model&& other);
      Model& operator=(Model& other);
      Model& operator=(Model&& other);
      friend void swap(Model& first, Model& second); // nothrow

      std::shared_ptr<Mesh> mesh = nullptr;
      Material material;
      Transform transform = Transform();

      void Draw(glm::mat4x4 view, glm::mat4x4 projection) const;

  }; // class Model

} // namespace dg
