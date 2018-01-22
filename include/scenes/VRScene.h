//
//  scenes/VRScene.h
//

#pragma once

#include <memory>
#include <Scene.h>
#include <Camera.h>
#include <Shader.h>
#include <Transform.h>
#include <Model.h>
#include <materials/StandardMaterial.h>
#include <Light.h>
#include <lights/SpotLight.h>
#include <openvr.h>

namespace dg {

  class VRScene : public Scene {

    public:

      static std::unique_ptr<VRScene> Make();

      VRScene();
      virtual ~VRScene();

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

      std::shared_ptr<SceneObject> leftController;
      std::shared_ptr<SceneObject> rightController;

      vr::IVRSystem *vrSystem;

  }; // class VRScene

} // namespace dg
