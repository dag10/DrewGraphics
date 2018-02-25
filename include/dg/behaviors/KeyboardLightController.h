//
//  behaviors/KeyboardLightController.h
//

#pragma once

#include <memory>
#include "dg/Behavior.h"
#include "dg/InputCodes.h"

namespace dg {

  class Window;
  class Light;

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

