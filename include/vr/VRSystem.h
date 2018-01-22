//
//  vr/VRSystem.h
//

#pragma once

#include <memory>
#include <openvr.h>

namespace dg {

  class VRSystem {

    public:

      static std::unique_ptr<VRSystem> Instance;

      static void Initialize();

      vr::IVRSystem *vrSystem = nullptr;

      virtual ~VRSystem();

    private:

      void StartOpenVR();

  }; // class VRSystem

} // namespace dg
