//
//  materials/CheckerboardMaterial.h
//

#pragma once

#include <Material.h>
#include <Shader.h>
#include <Texture.h>
#include <memory>

namespace dg {

  class CheckerboardMaterial : public Material {

    public:

      CheckerboardMaterial();

      CheckerboardMaterial(CheckerboardMaterial& other);
      CheckerboardMaterial(CheckerboardMaterial&& other);
      CheckerboardMaterial& operator=(CheckerboardMaterial& other);
      CheckerboardMaterial& operator=(CheckerboardMaterial&& other);
      friend void swap(CheckerboardMaterial &first,
                       CheckerboardMaterial &second); // nothrow

      void Use() const;

      void SetSize(glm::vec2 size);
      void SetColorA(glm::vec3 color);
      void SetColorB(glm::vec3 color);

    protected:

      void SetDefaultProperties();

      static std::shared_ptr<Shader> checkerboardShader;

  }; // class CheckerboardMaterial

} // namespace dg
