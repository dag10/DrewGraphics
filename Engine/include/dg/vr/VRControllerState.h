//
//  vr/VRControllerState.h
//

#pragma once

#include <openvr.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include "dg/Behavior.h"

namespace dg {

  class VRTrackedObject;

  class VRControllerState : public Behavior {

    public:

      enum class Button {
        // Vive and Touch
        TRIGGER,
        GRIP,
        MENU,
        SYSTEM,

        // Vive
        TOUCHPAD,

        // Oculus Touch
        JOYSTICK,
        A,
        B,
        X,
        Y,

        LAST, // Bookend, not a button.
      };

      enum class Axis {
        // Vive and Touch
        TRIGGER,

        // Vive
        TOUCHPAD,

        // Oculus Touch
        JOYSTICK,

        LAST, // Bookend, not an axis.
      };

      virtual void Initialize();
      virtual void Start();
      virtual void Update();

      bool IsButtonPressed(Button button) const;
      bool IsButtonJustPressed(Button button) const;
      bool IsButtonJustUnpressed(Button button) const;
      bool IsButtonTouched(Button button) const;
      bool IsButtonJustTouched(Button button) const;
      bool IsButtonJustUntouched(Button button) const;
      glm::vec2 GetAxis(Axis axis) const;
      const char *GetButtonName(Button button) const;

    private:

      static vr::EVRButtonId GetButtonID(Button button);
      static vr::EVRControllerAxisType GetAxisType(Axis axis);
      void ResetState();

      std::weak_ptr<VRTrackedObject> trackedObject;

      unsigned int lastPacketNum = 0;

      std::vector<uint64_t> buttonMasks;
      std::vector<bool> buttonPressed;
      std::vector<bool> buttonWasPressed;
      std::vector<bool> buttonTouched;
      std::vector<bool> buttonWasTouched;
      std::vector<glm::vec2> axisValues;

  }; // class VRControllerState

} // namespace dg
