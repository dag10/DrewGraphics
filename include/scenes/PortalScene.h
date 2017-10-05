//
//  scenes/PortalScene.h
//

#pragma once

#include <memory>
#include <Scene.h>
#include <Camera.h>
#include <Shader.h>
#include <Transform.h>
#include <Model.h>
#include <materials/StandardMaterial.h>
#include <PointLight.h>

namespace dg {

  class PortalScene : public Scene {

    public:

      static std::unique_ptr<PortalScene> Make();

      virtual void Initialize();
      virtual void Update();
      virtual void Render();

    private:

      void RenderScene(
          bool throughPortal, Transform inPortal, Transform outPortal);
      void RenderPortalStencil(Transform xfPortal);
      void ClearDepth();

      Camera camera;
      PointLight ceilingLight;
      bool animatingLight;
      std::vector<std::shared_ptr<Model>> models;
      std::shared_ptr<Model> lightModel;
      StandardMaterial portalStencilMaterial;
      std::shared_ptr<Shader> depthResetShader;

  }; // class PortalScene

} // namespace dg
