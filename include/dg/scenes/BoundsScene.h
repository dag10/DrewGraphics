//
//  scenes/BoundsScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class Model;

  class BoundsScene : public Scene {

    public:

      static std::unique_ptr<BoundsScene> Make();
      static std::unique_ptr<BoundsScene> MakeVR();

      virtual void Initialize();
      virtual void Update();

    private:

      BoundsScene(bool enableVR);

      std::shared_ptr<Model> spinningHelix;
      std::shared_ptr<Model> spinningTorus;

  }; // class BoundsScene

} // namespace dg
