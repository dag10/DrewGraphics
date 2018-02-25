//
//  scenes/VRScene.h
//

#pragma once

#include <openvr.h>
#include <memory>
#include "Scene.h"

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

  }; // class VRScene

} // namespace dg
