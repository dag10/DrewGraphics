//
//  behaviors/KeyboardLightController.h
//

#pragma once

#include <Behavior.h>
#include <Light.h>
#include <Window.h>
#include <InputCodes.h>
#include <memory>

namespace dg {

  class KeyboardLightController : public Behavior {

    public:

      KeyboardLightController(std::weak_ptr<Window> window);
      KeyboardLightController(
          std::weak_ptr<Window> window,
          Key ambientModifierKey, Key diffuseModifierKey,
          Key specularModifierKey);

      Key ambientModifierKey  = Key::NUM_1;
      Key diffuseModifierKey  = Key::NUM_2;
      Key specularModifierKey = Key::NUM_3;

      virtual void Update();

    private:

      std::weak_ptr<Window> window;

  }; // class KeyboardLightController

} // namespace dg

