//
//  scenes/RobotScene.h
//

#pragma once

#include <memory>
#include "Scene.h"

namespace dg {

  class RobotScene : public Scene {

    public:

      static std::unique_ptr<RobotScene> Make();
      static std::unique_ptr<RobotScene> MakeVR();

      virtual void Initialize();
      virtual void Update();

    private:

      RobotScene(bool enableVR);

      virtual void ClearBuffer();

      std::shared_ptr<SceneObject> lightContainer;

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

      // Control state.
      bool freeFly = false;

  }; // class RobotScene

} // namespace dg
