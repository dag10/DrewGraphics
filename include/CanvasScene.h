//
//  CanvasScene.h
//

#pragma once

#include <memory>
#include <Canvas.h>
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

      std::shared_ptr<Canvas> canvas = nullptr;
      std::shared_ptr<ScreenQuadMaterial> quadMaterial = nullptr;

  }; // class CanvasScene

} // namespace dg
