//
//  scenes/ShadowScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class ShadowScene : public Scene {

    public:

      static std::unique_ptr<ShadowScene> Make();

      ShadowScene();

      virtual void Initialize();

    protected:

      virtual void ClearBuffer();

  }; // class ShadowScene

} // namespace dg
