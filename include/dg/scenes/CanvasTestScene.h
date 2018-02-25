//
//  scenes/CanvasTestScene.h
//

#pragma once

#include "dg/CanvasScene.h"

namespace dg {

  class CanvasTestScene : public CanvasScene {

    public:

      static std::unique_ptr<CanvasTestScene> Make();

      CanvasTestScene();

      virtual void Initialize();

  }; // class CanvasTestScene

} // namespace dg
