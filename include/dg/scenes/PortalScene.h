//
//  scenes/PortalScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"
#include "dg/materials/StandardMaterial.h"

namespace dg {

  class Camera;
  class Light;
  class SpotLight;
  class DirectionalLight;
  class Shader;

  class PortalScene : public Scene {

    public:

      static std::unique_ptr<PortalScene> Make();

      PortalScene();

      virtual void Initialize();
      virtual void Update();
      virtual void RenderFrame();

    protected:
      virtual void DrawScene(
        const Camera& camera, bool renderForVR = false,
        vr::EVREye eye = vr::EVREye::Eye_Left);

    private:

      enum LightingType {
        OutdoorLighting,
        PointLighting,
        SpotLighting,
        FlashlightLighting,
      };

      void UpdateLightingConfiguration();

      void RenderPortalStencil(Transform xfPortal);
      void ClearDepth();
      Camera CameraForPortal(Transform inPortal, Transform outPortal);

      // Pipeline functions for overriding in special cases.
      virtual void PrepareModelForDraw(
          const Model& model,
          glm::vec3 cameraPosition,
          glm::mat4x4 view,
          glm::mat4x4 projection,
          const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) const;

      bool animatingLight;
      LightingType lightingType;
      glm::mat4x4 invPortal;
      std::shared_ptr<Model> ceiling;
      std::shared_ptr<Light> skyLight;
      std::shared_ptr<Model> lightModel;
      std::shared_ptr<Light> indoorCeilingLight;
      std::shared_ptr<Light> outdoorCeilingLight;
      std::shared_ptr<SpotLight> spotLight;
      std::shared_ptr<SpotLight> flashlight;
      std::shared_ptr<StandardMaterial> portalStencilMaterial;
      std::shared_ptr<Shader> depthResetShader;

  }; // class PortalScene

} // namespace dg
