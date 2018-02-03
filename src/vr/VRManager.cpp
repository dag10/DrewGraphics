//
//  vr/VRManager.cpp
//

#include <vr/VRManager.h>
#include <vr/VRTrackedObject.h>
#include <Exceptions.h>
#include <SceneObject.h>
#include <MathUtils.h>

dg::VRManager *dg::VRManager::Instance = nullptr;

void dg::VRManager::Initialize() {
  Behavior::Initialize();

  if (Instance != nullptr) {
    throw std::runtime_error("More than one VRManager was initialized.");
  }

  Instance = this;
  StartOpenVR();
}

void dg::VRManager::StartOpenVR() {
  if (!vr::VR_IsRuntimeInstalled()) {
    throw std::runtime_error("Please install the OpenVR Runtime.");
  }

  if (!vr::VR_IsHmdPresent()) {
    throw std::runtime_error("No VR headset is detected.");
  }

  vr::HmdError hmdError;
  vrSystem = vr::VR_Init(
    &hmdError, vr::EVRApplicationType::VRApplication_Scene);
  if (vrSystem == nullptr) {
    throw OpenVRError(hmdError);
  }

  vrCompositor = vr::VRCompositor();
}

dg::VRManager::~VRManager() {
  Behavior::~Behavior();

  Instance = nullptr;

  vr::VR_Shutdown();
  vrSystem = nullptr;
  vrCompositor = nullptr;
}

void dg::VRManager::ReadyToRender() {
  // Block until OpenVR's "running start" thinks we should begin our render
  // commands. This also loads in the tracked device poses for the upcoming
  // render and predicted poses for the upcoming frame.
  vrCompositor->WaitGetPoses(
    poses.data(), poses.size(), nextPoses.data(), nextPoses.size());
  UpdatePoses();
}

void dg::VRManager::RenderFinished() {
  // At this point poses still contains the render poses, and nextPoses
  // contains the gamePoses for the upcoming frame.
  // We'll swap poses with nextPoses to make poses be the poses relevant
  // for the next frame, and we'll re-parse the new poses to update
  // controller positions, etc.
  std::swap(poses, nextPoses);
  UpdatePoses();
}

void dg::VRManager::UpdatePoses() {
  // If we previously had a controller device index, and that device is no
  // longer connected, we'll next try to find another connected device to use.
  if (leftControllerIndex != -1 &&
    !poses[leftControllerIndex].bDeviceIsConnected) {
    leftControllerIndex = -1;
  }
  if (rightControllerIndex != -1 &&
    !poses[rightControllerIndex].bDeviceIsConnected) {
    rightControllerIndex = -1;
  }

  // Associate a device with the left controller if needed.
  if (leftControllerIndex == -1) {
    for (int i = 1; i < (int)poses.size(); i++) {
      if (!poses[i].bDeviceIsConnected) continue;
      if (vrSystem->GetTrackedDeviceClass(i) !=
        vr::TrackedDeviceClass_Controller) continue;
      if (i == rightControllerIndex) continue;

      leftControllerIndex = i;
      break;
    }
  }
  if (rightControllerIndex == -1) {
    for (int i = 1; i < (int)poses.size(); i++) {
      if (!poses[i].bDeviceIsConnected) continue;
      if (vrSystem->GetTrackedDeviceClass(i) !=
        vr::TrackedDeviceClass_Controller) continue;
      if (i == leftControllerIndex) continue;

      rightControllerIndex = i;
      break;
    }
  }

  // Update registered VRTrackedObject transforms.
  for (
    auto iter = trackedObjects.begin();
    iter != trackedObjects.end();
    iter++) {

    auto trackedObject = *iter;
    auto trackedSceneObject = trackedObject->SceneObject();

    if (trackedObject->role ==
      vr::ETrackedControllerRole::TrackedControllerRole_Invalid) {
      if (trackedObject->deviceIndex == -1) {
        continue;
      }
      trackedSceneObject->transform = Transform(HmdMat2Glm(
        poses[trackedObject->deviceIndex].mDeviceToAbsoluteTracking));
    } else {
      int index = (trackedObject->role ==
        vr::ETrackedControllerRole::TrackedControllerRole_LeftHand)
        ? leftControllerIndex
        : rightControllerIndex;

      trackedObject->deviceIndex = index;
      if (index == -1) {
        trackedSceneObject->enabled = false;
      } else {
        trackedSceneObject->enabled = true;
        trackedSceneObject->transform = Transform(HmdMat2Glm(
          poses[index].mDeviceToAbsoluteTracking));
      }
    }
  }
}

void dg::VRManager::RegisterTrackedObject(VRTrackedObject *object) {
  trackedObjects.push_front(object);
}

void dg::VRManager::DeregisterTrackedObject(VRTrackedObject *object) {
  trackedObjects.remove(object);
}
