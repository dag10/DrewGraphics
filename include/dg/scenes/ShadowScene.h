//
//  scenes/ShadowScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class FrameBuffer;
  class ScreenQuadMaterial;
  class SpotLight;

  class ShadowScene : public Scene {

    public:

      static std::unique_ptr<ShadowScene> Make();

      ShadowScene();

      virtual void Initialize();
      virtual void Update();

    private:

      virtual void RenderFrame();
      virtual void PrepareModelForDraw(
          const Model& model,
          glm::vec3 cameraPosition,
          glm::mat4x4 view,
          glm::mat4x4 projection,
          const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) const;

      std::shared_ptr<Model> cube = nullptr;
      std::shared_ptr<FrameBuffer> framebuffer = nullptr;
      std::shared_ptr<SpotLight> spotlight = nullptr;
      std::shared_ptr<ScreenQuadMaterial> quadMaterial = nullptr;

      // TODO: Refactor this
      glm::mat4 lightTransform = glm::mat4(0);
      bool renderingDepthOnly = false;

  }; // class ShadowScene

} // namespace dg
