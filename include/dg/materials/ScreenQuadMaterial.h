//
//  materials/ScreenQuadMaterial.h
//
#pragma once

#include <memory>
#include "dg/Material.h"
#include "dg/Shader.h"
#include "dg/Texture.h"

namespace dg {

  class ScreenQuadMaterial : public Material {

    public:

      ScreenQuadMaterial();
      ScreenQuadMaterial(
        glm::vec3 color, glm::vec2 scale = glm::vec2(1),
        glm::vec2 offset = glm::vec2(0));
      ScreenQuadMaterial(
        std::shared_ptr<Texture> texture, glm::vec2 scale = glm::vec2(1),
        glm::vec2 offset = glm::vec2(0));

      ScreenQuadMaterial(ScreenQuadMaterial& other);
      ScreenQuadMaterial(ScreenQuadMaterial&& other);
      ScreenQuadMaterial& operator=(ScreenQuadMaterial& other);
      ScreenQuadMaterial& operator=(ScreenQuadMaterial&& other);
      friend void swap(ScreenQuadMaterial& first, ScreenQuadMaterial& second); // nothrow

      void Use() const;

      void SetColor(glm::vec3 color);
      void SetTexture(std::shared_ptr<Texture> texture);
      void SetScale(glm::vec2 scale);
      void SetOffset(glm::vec2 offset);
      void SetRedChannelOnly(bool useRedChannelOnly);

    private:

      static std::shared_ptr<Shader> screenQuadShader;

  }; // class ScreenQuadMaterial

} // namespace dg

