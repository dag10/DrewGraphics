//
//  scenes/DeepCloningScene.h
//

#pragma once

#include <memory>
#include <Scene.h>

namespace dg {

  class DeepCloningScene : public Scene {

    public:

      static std::unique_ptr<DeepCloningScene> Make();

      DeepCloningScene();

      virtual void Initialize();

    private:

  }; // class DeepCloningScene

} // namespace dg
