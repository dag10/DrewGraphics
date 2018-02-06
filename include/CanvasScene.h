//
//  CanvasScene.h
//

#pragma once

#include <memory>
#include <Texture.h>
#include <materials/ScreenQuadMaterial.h>
#include <Scene.h>

namespace dg {

  class CanvasScene : public Scene {

    public:

      CanvasScene();

      virtual void Initialize();
      virtual void RenderFrame();

    protected:

      virtual void ConfigureBuffer();

      std::shared_ptr<Texture> canvas; // TODO: Will become a Canvas.
      std::shared_ptr<ScreenQuadMaterial> quadMaterial;

  }; // class CanvasScene

} // namespace dg
