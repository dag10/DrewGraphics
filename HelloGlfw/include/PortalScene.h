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

      std::shared_ptr<dg::Mesh> cubeMesh;
      std::shared_ptr<dg::Mesh> quadMesh;
      dg::Camera camera;
      dg::Shader simpleTextureShader;
      dg::Shader solidColorShader;
      dg::Shader depthResetShader;
      dg::Texture crateTexture;

  }; // class PortalScene

} // namespace dg
