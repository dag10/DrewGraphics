//
//  scenes/VRScene.h
//

#pragma once

#include <openvr.h>
#include <memory>
#include "dg/Scene.h"

namespace dg {

  class Model;
  class Light;
  class FrameBuffer;
  class SpotLight;

  class VRScene : public Scene {

    public:

      static std::unique_ptr<VRScene> Make();

      VRScene();

      virtual void Initialize();
      virtual void Update();

    private:

      enum LightingType {
        OutdoorLighting,
        PointLighting,
        SpotLighting,
        FlashlightLighting,
      };

      virtual void RenderFrame();
      virtual void PrepareModelForDraw(
          const Model& model,
          glm::vec3 cameraPosition,
          glm::mat4x4 view,
          glm::mat4x4 projection,
          const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) const;
      void UpdateLightingConfiguration();
      virtual void ConfigureBuffer();

      bool animatingLight;
      LightingType lightingType;
      std::shared_ptr<Model> ceiling;
      std::shared_ptr<Light> skyLight;
      std::shared_ptr<Model> lightModel;
      std::shared_ptr<Light> indoorCeilingLight;
      std::shared_ptr<Light> outdoorCeilingLight;
      std::shared_ptr<SpotLight> spotLight;
      std::shared_ptr<SpotLight> flashlight;
      std::shared_ptr<FrameBuffer> framebuffer = nullptr;
      std::shared_ptr<ScreenQuadMaterial> vrQuadMaterial = nullptr;
      glm::mat4 lightTransform = glm::mat4(0);

  }; // class VRScene

} // namespace dg
