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

      virtual bool AutomaticWindowTitle() const;

    private:

      void Raytrace();

      std::shared_ptr<ScreenQuadMaterial> quadMaterial = nullptr;
      Transform renderCameraTransform;
      bool showRender = false;
      bool raytraceNextFrame = false;

  }; // class RaytraceScene

} // namespace dg
