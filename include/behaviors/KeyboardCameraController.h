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

      KeyboardCameraController(std::weak_ptr<Window> window);
      KeyboardCameraController(std::weak_ptr<Window> window, float speed);

      float speed = 1.8f; // units per second

      virtual void Start();
      virtual void Update();

    private:

      std::weak_ptr<Window> window;

      Transform originalTransform;

  }; // class KeyboardCameraController

} // namespace dg

