//
//  Model.h
//
#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Mesh.h"

namespace dg {

  class Model {

    public:

      Model(
          std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader,
          std::shared_ptr<Texture> texture, glm::vec2 uvScale,
          Transform transform);

      Model(Model& other);
      Model(Model&& other);
      Model& operator=(Model& other);
      Model& operator=(Model&& other);
      friend void swap(Model& first, Model& second); // nothrow

      std::shared_ptr<Mesh> mesh = nullptr;
      std::shared_ptr<Shader> shader = nullptr;
      Transform transform = Transform();

      // TODO: Don't hard-code these into the model. Use a vector of shader
      //       properties instead.
      std::shared_ptr<Texture> texture = nullptr;
      glm::vec2 uvScale = glm::vec2(1);
      glm::mat4x4 invPortal = glm::mat4x4(0);

      void Draw(glm::mat4x4 view, glm::mat4x4 projection) const;

  }; // class Model

} // namespace dg
