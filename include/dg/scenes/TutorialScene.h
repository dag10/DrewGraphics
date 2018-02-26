//
//  scenes/TutorialScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class TutorialScene : public Scene {

    public:

      static std::unique_ptr<TutorialScene> Make();

      TutorialScene();

      virtual void Initialize();

    protected:

      virtual void ClearBuffer();

  }; // class TutorialScene

} // namespace dg
