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

        NumLightingModes,
      };

      void UpdateLightingConfiguration();

      bool animatingLight;
      LightingType lightingType;
      std::shared_ptr<Model> ceiling;
      std::shared_ptr<Light> skyLight;
      std::shared_ptr<Model> lightModel;
      std::shared_ptr<Light> indoorCeilingLight;
      std::shared_ptr<Light> outdoorCeilingLight;
      std::shared_ptr<SpotLight> spotLight;
      std::shared_ptr<SpotLight> flashlight;
      std::shared_ptr<SceneObject> leftController;
      std::shared_ptr<SceneObject> rightController;
      glm::vec3 originalFlashlightDiffuse;
      glm::vec3 originalFlashlightSpecular;

  }; // class VRScene

} // namespace dg
