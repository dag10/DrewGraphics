//
//  scenes/AOScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class AOScene : public Scene {

    public:

      static std::unique_ptr<AOScene> Make();

      AOScene();

      virtual void Initialize();

  }; // class AOScene

} // namespace dg
