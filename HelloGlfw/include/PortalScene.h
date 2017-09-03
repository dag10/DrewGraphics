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
      virtual void Render(dg::Window& window);

    private:

      void RenderScene(
          dg::Window& window, bool throughPortal,
          dg::Transform inPortal, dg::Transform outPortal);

      std::shared_ptr<dg::Mesh> cubeMesh;
      std::shared_ptr<dg::Mesh> quadMesh;
      dg::Camera camera;
      dg::Shader simpleTextureShader;
      dg::Shader solidColorShader;
      dg::Texture crateTexture;

  }; // class PortalScene

} // namespace dg
