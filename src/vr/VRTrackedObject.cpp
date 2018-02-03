//
//  vr/VRTrackedObject.cpp
//

#include <vr/VRTrackedObject.h>
#include <vr/VRManager.h>

dg::VRTrackedObject::VRTrackedObject(vr::ETrackedControllerRole role)
  : role(role), Behavior() {}

dg::VRTrackedObject::VRTrackedObject(int deviceIndex)
  : role(vr::ETrackedControllerRole::TrackedControllerRole_Invalid),
  deviceIndex(deviceIndex), Behavior() {}

dg::VRTrackedObject::~VRTrackedObject() {
  Behavior::~Behavior();
  VRManager::Instance->DeregisterTrackedObject(this);
}

void dg::VRTrackedObject::Initialize() {
  Behavior::Initialize();
  VRManager::Instance->RegisterTrackedObject(this);
}
