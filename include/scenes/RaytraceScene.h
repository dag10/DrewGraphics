//
//  scenes/RaytraceScene.h
//

#pragma once

#include <memory>
#include <Scene.h>
#include <Transform.h>

namespace dg {

  class ScreenQuadMaterial;

  class RaytraceScene : public Scene {

    public:

      static std::unique_ptr<RaytraceScene> Make();

      RaytraceScene();

      virtual void Initialize();
      virtual void Update();
      virtual void RenderFrame();

    private:

      std::shared_ptr<ScreenQuadMaterial> quadMaterial = nullptr;
      Transform renderCameraTransform;
      bool showRender = false;

  }; // class RaytraceScene

} // namespace dg
