//
//  scenes/ShadowScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class ScreenQuadMaterial;
  class SpotLight;

  class ShadowScene : public Scene {

    public:

      static std::unique_ptr<ShadowScene> Make();

      ShadowScene();

      virtual void Initialize();
      virtual void Update();

    private:

      virtual void RenderOverlays();

      std::shared_ptr<Model> cube = nullptr;
      std::shared_ptr<SpotLight> spotlight = nullptr;
      std::shared_ptr<ScreenQuadMaterial> quadMaterial = nullptr;

  }; // class ShadowScene

} // namespace dg
