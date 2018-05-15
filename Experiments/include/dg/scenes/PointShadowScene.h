//
//  scenes/PointShadowScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class SpotLight;

  class PointShadowScene : public Scene {

    public:

      static std::unique_ptr<PointShadowScene> Make();
      static std::unique_ptr<PointShadowScene> MakeVR();

      virtual void Initialize();
      virtual void Update();

    private:

      PointShadowScene(bool enableVR);

      virtual void ClearBuffer();

      std::shared_ptr<SceneObject> lightContainer;
      std::shared_ptr<SpotLight> flashlight;

      // Robot joints.
      std::shared_ptr<SceneObject> robot;
      std::shared_ptr<SceneObject> leftShoulder;
      std::shared_ptr<SceneObject> rightShoulder;
      std::shared_ptr<SceneObject> leftElbow;
      std::shared_ptr<SceneObject> rightElbow;
      std::shared_ptr<SceneObject> neck;
      std::shared_ptr<SceneObject> eyes;

      // Robot state.
      float nextBlink = -1;
      float endOfBlink = -1;

  }; // class PointShadowScene

} // namespace dg
