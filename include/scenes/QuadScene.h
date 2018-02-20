//
//  scenes/QuadScene.h
//

#pragma once

#include <memory>
#include <Scene.h>

namespace dg {

  class Model;

  class QuadScene : public Scene {

    public:

      static std::unique_ptr<QuadScene> Make();

      QuadScene();

      virtual void Initialize();
      virtual void Update();

    private:

      std::shared_ptr<Model> quad;

  }; // class QuadScene

} // namespace dg
