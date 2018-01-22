//
//  vr/VRSystem.cpp
//

#pragma once

#include <vr/VRSystem.h>
#include <Exceptions.h>

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

  vr::HmdError error;
  vrSystem = vr::VR_Init(&error, vr::EVRApplicationType::VRApplication_Scene);
  if (vrSystem == nullptr) {
    throw OpenVRError(error);
  }
}

dg::VRSystem::~VRSystem() {
  vr::VR_Shutdown();
  vrSystem = nullptr;
}
