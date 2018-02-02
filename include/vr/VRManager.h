//
//  vr/VRManager.h
//

#pragma once

#include <Transform.h>
#include <memory>
#include <vector>
#include <openvr.h>

namespace dg {

  class VRManager {

    public:

      static std::unique_ptr<VRManager> Instance;

      static void Initialize();

      vr::IVRSystem *vrSystem = nullptr;
      vr::IVRCompositor *vrCompositor = nullptr;

      virtual ~VRManager();

      void WaitGetPoses();

      const Transform *GetHmdTransform() const;
      const Transform *GetLeftControllerTransform() const;
      const Transform *GetRightControllerTransform() const;

    private:

      struct TrackedDeviceInfo {
        int deviceIndex = -1;
        Transform transform;
      };

      void StartOpenVR();

      TrackedDeviceInfo hmd;
      TrackedDeviceInfo leftController;
      TrackedDeviceInfo rightController;

      vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];

  }; // class VRManager

} // namespace dg
