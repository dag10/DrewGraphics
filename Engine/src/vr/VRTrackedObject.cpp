//
//  vr/VRTrackedObject.cpp
//

#include "dg/vr/VRTrackedObject.h"
#include <iostream>
#include "dg/vr/VRManager.h"

dg::VRTrackedObject::VRTrackedObject(vr::ETrackedControllerRole role)
  : Behavior(), role(role) {}

dg::VRTrackedObject::VRTrackedObject(int deviceIndex)
  : Behavior(), role(vr::ETrackedControllerRole::TrackedControllerRole_Invalid),
  deviceIndex(deviceIndex) {}

dg::VRTrackedObject::~VRTrackedObject() {
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