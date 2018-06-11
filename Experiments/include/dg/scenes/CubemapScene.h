//
//  scenes/CubemapScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class Model;
  class RotateBehavior;

  class CubemapScene : public Scene {

    public:

      static std::unique_ptr<CubemapScene> Make();
      static std::unique_ptr<CubemapScene> MakeVR();

      virtual void Initialize();
      virtual void Update();

    private:

      CubemapScene(bool enableVR);

      std::shared_ptr<RotateBehavior> cameraRotateBehavior;

  }; // class CubemapScene

} // namespace dg
