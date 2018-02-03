//
//  vr/VRTrackedObject.cpp
//

#include <vr/VRTrackedObject.h>
#include <vr/VRManager.h>
#include <iostream>

dg::VRTrackedObject::VRTrackedObject(vr::ETrackedControllerRole role)
  : role(role), Behavior() {}

dg::VRTrackedObject::VRTrackedObject(int deviceIndex)
  : role(vr::ETrackedControllerRole::TrackedControllerRole_Invalid),
  deviceIndex(deviceIndex), Behavior() {}

dg::VRTrackedObject::~VRTrackedObject() {
  Behavior::~Behavior();
  if (VRManager::Instance != nullptr) {
    VRManager::Instance->DeregisterTrackedObject(this);
  }
}

void dg::VRTrackedObject::Initialize() {
  Behavior::Initialize();
  if (VRManager::Instance == nullptr) {
    std::cerr << "Warning: VRTrackedObject could not find an active VRManager."
      << std::endl;
    enabled = false;
    return;
  }
  VRManager::Instance->RegisterTrackedObject(this);
}
