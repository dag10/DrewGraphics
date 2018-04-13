//
//  vr/VRControllerState.cpp
//

#include "dg/vr/VRControllerState.h"
#include "dg/SceneObject.h"
#include "dg/vr/VRManager.h"
#include "dg/vr/VRTrackedObject.h"

void dg::VRControllerState::Initialize() {
  buttonMasks = std::vector<uint64_t>((int)Button::LAST);
  buttonPressed = std::vector<bool>((int)Button::LAST);
  buttonWasPressed = std::vector<bool>((int)Button::LAST);
  buttonTouched = std::vector<bool>((int)Button::LAST);
  buttonWasTouched = std::vector<bool>((int)Button::LAST);
  axisValues = std::vector<glm::vec2>((int)Axis::LAST);

  for (int i = 0; i < (int)Button::LAST; i++) {
    buttonMasks[i] = vr::ButtonMaskFromId(GetButtonID((Button)i));
  }

  ResetState();
}

void dg::VRControllerState::Start() {
  Behavior::Start();
  trackedObject = GetSceneObject()->GetBehavior<VRTrackedObject>();
}

void dg::VRControllerState::Update() {
  Behavior::Update();

  buttonWasPressed = buttonPressed;
  buttonWasTouched = buttonTouched;

  auto trackedObject = this->trackedObject.lock();
  if (!trackedObject || !trackedObject->enabled) {
    ResetState();
    return;
  }

  if (trackedObject->deviceIndex == -1) {
    ResetState();
    return;
  }

  vr::VRControllerState001_t state = {};
  bool success = VRManager::Instance->vrSystem->GetControllerState(
    trackedObject->deviceIndex, &state, sizeof(state));
  if (!success) {
    return;
  }

  if (state.unPacketNum <= lastPacketNum) {
    // No changes to report.
    return;
  }

  lastPacketNum = state.unPacketNum;

  // Update button states.
  for (int i = 0; i < (int)Button::LAST; i++) {
    buttonPressed[i] = (state.ulButtonPressed & buttonMasks[i]) != 0;
    buttonTouched[i] = (state.ulButtonTouched & buttonMasks[i]) != 0;
  }

  // Update axis states.
  for (int i = 0; i < (int)Axis::LAST; i++) {
    axisValues[i] = glm::vec2(0);
  }
  for (int i = 0; i < vr::k_unControllerStateAxisCount; i++) {
    int32_t type = VRManager::Instance->vrSystem->GetInt32TrackedDeviceProperty(
        trackedObject->deviceIndex,
        (vr::ETrackedDeviceProperty)((int)vr::Prop_Axis0Type_Int32 + i));
    for (int j = 0; j < (int)Axis::LAST; j++) {
      if (type == (int32_t)GetAxisType((Axis)j)) {
        axisValues[j] = glm::vec2(state.rAxis[i].x, state.rAxis[i].y);
      }
    }
  }
}

bool dg::VRControllerState::IsButtonPressed(Button button) const {
  if ((int)button < 0 || (unsigned int)button >= buttonPressed.size()) {
    return false;
  }

  return buttonPressed[(int)button];
}

bool dg::VRControllerState::IsButtonJustPressed(Button button) const {
  if ((int)button < 0 || (unsigned int)button >= buttonPressed.size()) {
    return false;
  }

  return buttonPressed[(int)button] && !buttonWasPressed[(int)button];
}

bool dg::VRControllerState::IsButtonJustUnpressed(Button button) const {
  if ((int)button < 0 || (unsigned int)button >= buttonPressed.size()) {
    return false;
  }

  return !buttonPressed[(int)button] && buttonWasPressed[(int)button];
}

bool dg::VRControllerState::IsButtonTouched(Button button) const {
  if ((int)button < 0 || (unsigned int)button >= buttonTouched.size()) {
    return false;
  }

  return buttonTouched[(int)button];
}

bool dg::VRControllerState::IsButtonJustTouched(Button button) const {
  if ((int)button < 0 || (unsigned int)button >= buttonTouched.size()) {
    return false;
  }

  return buttonTouched[(int)button] && !buttonWasTouched[(int)button];
}

bool dg::VRControllerState::IsButtonJustUntouched(Button button) const {
  if ((int)button < 0 || (unsigned int)button >= buttonTouched.size()) {
    return false;
  }

  return !buttonTouched[(int)button] && buttonWasTouched[(int)button];
}

glm::vec2 dg::VRControllerState::GetAxis(Axis axis) const {
  if ((int)axis < 0 || (unsigned int)axis >= axisValues.size()) {
    return glm::vec2(0);
  }

  return axisValues[(int)axis];
}

const char *dg::VRControllerState::GetButtonName(Button button) const {
  return VRManager::Instance->vrSystem->GetButtonIdNameFromEnum(
      GetButtonID(button));
}

vr::EVRButtonId dg::VRControllerState::GetButtonID(Button button) {
  switch (button) {
    case Button::TRIGGER:
      return vr::EVRButtonId::k_EButton_SteamVR_Trigger;
    case Button::GRIP:
      return vr::EVRButtonId::k_EButton_Grip;
    case Button::MENU:
    case Button::B:
    case Button::Y:
      return vr::EVRButtonId::k_EButton_ApplicationMenu;
    case Button::SYSTEM:
      return vr::EVRButtonId::k_EButton_System;
    case Button::TOUCHPAD:
    case Button::JOYSTICK:
      return vr::EVRButtonId::k_EButton_SteamVR_Touchpad;
    case Button::A:
    case Button::X:
      return (vr::EVRButtonId)((int)vr::EVRButtonId::k_EButton_A);
    case Button::LAST:
      break;
  }
  return (vr::EVRButtonId)0;
}

vr::EVRControllerAxisType dg::VRControllerState::GetAxisType(Axis axis) {
  switch (axis) {
    case Axis::TRIGGER:
      return vr::EVRControllerAxisType::k_eControllerAxis_Trigger;
    case Axis::TOUCHPAD:
      return vr::EVRControllerAxisType::k_eControllerAxis_TrackPad;
    case Axis::JOYSTICK:
      return vr::EVRControllerAxisType::k_eControllerAxis_Joystick;
    case Axis::LAST:
      break;
  }
  return (vr::EVRControllerAxisType)0;
}

void dg::VRControllerState::ResetState() {
  buttonWasPressed = buttonPressed;
  for (int i = 0; i < (int)Button::LAST; i++) {
    buttonPressed[i] = false;
  }
  for (int i = 0; i < (int)Axis::LAST; i++) {
    axisValues[i] = glm::vec2(0);
  }
}
