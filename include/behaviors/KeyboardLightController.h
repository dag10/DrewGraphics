//
//  behaviors/KeyboardLightController.h
//

#pragma once

#include <Behavior.h>
#include <Light.h>
#include <Window.h>
#include <memory>

namespace dg {

  class KeyboardLightController : public Behavior {

    public:

      KeyboardLightController(std::weak_ptr<Window> window);
      KeyboardLightController(
          std::weak_ptr<Window> window,
          int ambientModifierKey, int diffuseModifierKey,
          int specularModifierKey);

      int ambientModifierKey  = GLFW_KEY_1;
      int diffuseModifierKey  = GLFW_KEY_2;
      int specularModifierKey = GLFW_KEY_3;

      virtual void Update();

    private:

      std::weak_ptr<Window> window;

  }; // class KeyboardLightController

} // namespace dg

