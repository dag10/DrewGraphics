//
//  scenes/SimpleScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class SimpleScene : public Scene {

    public:

      static std::unique_ptr<SimpleScene> Make();

      SimpleScene();

      virtual void Initialize();

    private:

      virtual void ClearBuffer();

  }; // class SimpleScene

} // namespace dg
