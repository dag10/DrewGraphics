//
//  behaviors/KeyboardCameraController.h
//

#pragma once

#include <Behavior.h>
#include <Camera.h>
#include <Window.h>
#include <memory>

namespace dg {

  class KeyboardCameraController : public Behavior {

    public:

      KeyboardCameraController(
          std::weak_ptr<Camera> camera, std::weak_ptr<Window> window);

      virtual void Start();
      virtual void Update();

    private:

      std::weak_ptr<Camera> camera;
      std::weak_ptr<Window> window;

      Transform originalTransform;

  }; // class KeyboardCameraController

} // namespace dg

