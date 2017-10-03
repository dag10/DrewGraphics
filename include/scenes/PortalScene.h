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

namespace dg {

  class PortalScene : public Scene {

    public:

      static std::unique_ptr<PortalScene> Make();

      virtual void Initialize();
      virtual void Update();
      virtual void Render();

    private:

      void RenderScene(
          bool throughPortal, dg::Transform inPortal, dg::Transform outPortal);
      void RenderPortalStencil(dg::Transform xfPortal);
      void ClearDepth();

      dg::Camera camera;
      Transform xfLight;
      std::vector<Model> models;
      StandardMaterial portalStencilMaterial;
      std::shared_ptr<Shader> depthResetShader;

  }; // class PortalScene

} // namespace dg
