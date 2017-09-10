//
//  PortalScene.h
//

#pragma once

#include "Scene.h"
#include "Window.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Mesh.h"
#include "Model.h"
#include <memory>

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
      std::shared_ptr<Shader> simpleTextureShader;
      std::shared_ptr<Shader> solidColorShader;
      std::shared_ptr<Shader> depthResetShader;

  }; // class PortalScene

} // namespace dg
