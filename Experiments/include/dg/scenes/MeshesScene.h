//
//  scenes/MeshesScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class Model;

  class MeshesScene : public Scene {

    public:

      static std::unique_ptr<MeshesScene> Make();
      static std::unique_ptr<MeshesScene> MakeVR();

      virtual void Initialize();
      virtual void Update();

    private:

      MeshesScene(bool enableVR);

      std::shared_ptr<Model> spinningHelix;
      std::shared_ptr<Model> spinningTorus;

  }; // class MeshesScene

} // namespace dg
