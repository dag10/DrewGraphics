//
//  vr/VRSystem.cpp
//

#include <vr/VRSystem.h>
#include <Exceptions.h>
#include <MathUtils.h>

std::unique_ptr<dg::VRSystem> dg::VRSystem::Instance = nullptr;

void dg::VRSystem::Initialize() {
  auto newInstance = std::unique_ptr<VRSystem>(new VRSystem);
  newInstance->StartOpenVR();
  Instance = std::move(newInstance);
}

void dg::VRSystem::StartOpenVR() {
  if (!vr::VR_IsRuntimeInstalled()) {
    throw std::runtime_error("Please install the OpenVR Runtime.");
  }

  if (!vr::VR_IsHmdPresent()) {
    throw std::runtime_error("No VR headset is detected.");
  }

  vr::HmdError hmdError;
  vrSystem = vr::VR_Init(&hmdError, vr::EVRApplicationType::VRApplication_Scene);
  if (vrSystem == nullptr) {
    throw OpenVRError(hmdError);
  }

  vrCompositor = vr::VRCompositor();
}

dg::VRSystem::~VRSystem() {
  vr::VR_Shutdown();
  vrSystem = nullptr;
  vrCompositor = nullptr;
}

void dg::VRSystem::WaitGetPoses() {
  // Ignore render poses for now, just use game poses, which are for next frame.
  // TODO: Use both render poses and game poses.
  vrCompositor->WaitGetPoses(
    nullptr, 0, poses, sizeof(poses) / sizeof(poses[0]));

  // HMD is always device 0.
  if (poses[0].bPoseIsValid) {
    hmd.deviceIndex = 0;
    hmd.transform = Transform(HmdMat2Glm(poses[0].mDeviceToAbsoluteTracking));
  } else {
    hmd.deviceIndex = -1;
  }

  // Did we have a device index last frame?
  bool foundLeft = (leftController.deviceIndex >= 0);
  bool foundRight = (leftController.deviceIndex >= 0);

  // If we previously had a controller device index, and that device is no
  // longer connected, we'll next try to find another connected device to use.
  if (foundLeft && !poses[leftController.deviceIndex].bDeviceIsConnected) {
    foundLeft = false;
  }
  if (foundRight && !poses[rightController.deviceIndex].bDeviceIsConnected) {
    foundRight = false;
  }

  // Associate a device with the left controller if needed.
  if (!foundLeft) {
    for (int i = 1; i < sizeof(poses) / sizeof(poses[0]); i++) {
      if (!poses[i].bDeviceIsConnected) continue;
      if (vrSystem->GetTrackedDeviceClass(i) !=
        vr::TrackedDeviceClass_Controller) continue;
      if (i == rightController.deviceIndex) continue;

      leftController.deviceIndex = i;
      foundLeft = true;
      break;
    }
  }
  if (!foundRight) {
    for (int i = 1; i < sizeof(poses) / sizeof(poses[0]); i++) {
      if (!poses[i].bDeviceIsConnected) continue;
      if (vrSystem->GetTrackedDeviceClass(i) !=
        vr::TrackedDeviceClass_Controller) continue;
      if (i == leftController.deviceIndex) continue;

      rightController.deviceIndex = i;
      foundRight = true;
      break;
    }
  }

  // If we still have't found a device for a disconnected (or never connected)
  // controller, invalidate its transform.
  if (!foundLeft) {
    leftController.deviceIndex = -1;
  }
  if (!foundRight) {
    rightController.deviceIndex = -1;
  }

  // Update controllers' transforms.
  if (foundLeft) {
    leftController.transform = Transform(HmdMat2Glm(
      poses[leftController.deviceIndex].mDeviceToAbsoluteTracking));
  }
  if (foundRight) {
    rightController.transform = Transform(HmdMat2Glm(
      poses[rightController.deviceIndex].mDeviceToAbsoluteTracking));
  }
}

const dg::Transform *dg::VRSystem::GetHmdTransform() const {
  return hmd.deviceIndex >= 0 ? &hmd.transform : nullptr;
}

const dg::Transform *dg::VRSystem::GetLeftControllerTransform() const {
  return leftController.deviceIndex >= 0 ? &leftController.transform : nullptr;
}

const dg::Transform *dg::VRSystem::GetRightControllerTransform() const {
  return rightController.deviceIndex >= 0 ? &rightController.transform : nullptr;
}
