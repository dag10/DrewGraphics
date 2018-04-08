//
//  scenes/CaveTestScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace cavr {

  class Model;
  class Light;
  class SpotLight;

  class CaveTestScene : public dg::Scene {

    public:

      static std::unique_ptr<CaveTestScene> Make();

      virtual void Initialize();
      virtual void Update();

    private:

      CaveTestScene();

      std::shared_ptr<dg::Light> skyLight;

  }; // class CaveTestScene

} // namespace cavr
