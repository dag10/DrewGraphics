//
//  StandardMaterial.h
//
#pragma once

#include <memory>
#include "Material.h"
#include "Shader.h"
#include "Texture.h"

namespace dg {

  class StandardMaterial : public Material {

    public:

      static StandardMaterial WithColor(glm::vec3 color);
      static StandardMaterial WithColor(glm::vec4 color);
      static StandardMaterial WithTexture(std::shared_ptr<Texture> texture);

      StandardMaterial();

      StandardMaterial(StandardMaterial& other);
      StandardMaterial(StandardMaterial&& other);
      StandardMaterial& operator=(StandardMaterial& other);
      StandardMaterial& operator=(StandardMaterial&& other);
      friend void swap(StandardMaterial& first, StandardMaterial& second); // nothrow

      void Use() const;

      void SetUVScale(glm::vec2 scale);
      void SetLit(bool lit);
      void SetAlbedo(std::shared_ptr<Texture> value);
      void SetAlbedo(glm::vec3 value);
      void SetAlbedo(glm::vec4 value);
      void SetAmbient(float ambient);
      void SetDiffuse(float diffuse);
      void SetSpecular(float specular);

    private:

      static std::shared_ptr<Shader> standardShader;

  }; // class Material

} // namespace dg
