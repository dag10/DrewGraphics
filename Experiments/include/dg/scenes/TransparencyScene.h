//
//  scenes/TransparencyScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class Model;

  class TransparencyScene : public Scene {

    public:

      static std::unique_ptr<TransparencyScene> Make();
      static std::unique_ptr<TransparencyScene> MakeVR();

      virtual void Initialize();
      virtual void Update();

    private:

      TransparencyScene(bool enableVR);

      std::shared_ptr<Model> spinningHelix;
      std::shared_ptr<Model> spinningTorus;

  }; // class TransparencyScene

} // namespace dg
