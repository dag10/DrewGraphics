//
//  scenes/CubemapScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class Model;

  class CubemapScene : public Scene {

    public:

      static std::unique_ptr<CubemapScene> Make();
      static std::unique_ptr<CubemapScene> MakeVR();

      virtual void Initialize();

    private:

      CubemapScene(bool enableVR);

  }; // class CubemapScene

} // namespace dg
