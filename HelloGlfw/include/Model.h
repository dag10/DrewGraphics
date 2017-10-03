//
//  Model.h
//
#pragma once

#include <memory>
#include <glm/mat4x4.hpp>

#include <SceneObject.h>
#include <Material.h>
#include <Mesh.h>

namespace dg {

  class Model : public SceneObject {

    public:

      Model() = default;

      Model(
          std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
          Transform transform);

      Model(Model& other);
      Model(Model&& other);
      Model& operator=(Model& other);
      Model& operator=(Model&& other);
      friend void swap(Model& first, Model& second); // nothrow

      std::shared_ptr<Mesh> mesh = nullptr;
      std::shared_ptr<Material>  material = nullptr;

      void Draw(glm::mat4x4 view, glm::mat4x4 projection) const;

  }; // class Model

} // namespace dg
