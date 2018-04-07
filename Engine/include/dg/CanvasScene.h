//
//  CanvasScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class Canvas;
  class ScreenQuadMaterial;

  class CanvasScene : public Scene {

    public:

      CanvasScene();
      virtual ~CanvasScene();

      virtual void Initialize();
      virtual void RenderFrame();

    protected:

      std::shared_ptr<Canvas> canvas = nullptr;
      std::shared_ptr<ScreenQuadMaterial> quadMaterial = nullptr;

  }; // class CanvasScene

} // namespace dg
