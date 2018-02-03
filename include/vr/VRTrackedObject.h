//
//  vr/VRTrackedObject.h
//

#pragma once

#include <Behavior.h>
#include <openvr.h>

namespace dg {

  class VRTrackedObject : public Behavior {

    public:

      // When using this constructor, the object will automatically track to
      // the specified controller. Whenver the controller is not detected,
      // the attached SceneObject will be disabled. When a controller is
      // detected, its deviceIndex will be stored in the deviceIndex field.
      VRTrackedObject(vr::ETrackedControllerRole role);

      // When using this constructor, the associated SceneObject's position
      // will be the pose of the explicit deviceIndex.
      //
      // Pass in 0 to be tracked to the HMD's pose. In OpenVR, the HMD is
      // always deviceIndex 0.
      VRTrackedObject(int deviceIndex);

      virtual ~VRTrackedObject();

      virtual void Initialize();

      // If this is TrackedControllerRole_Invalid, this behavior will track
      // the sceneObject to the pose for the device given by deviceIndex.
      // If this role is set to either the left or right controller, the
      // sceneObject will track to whichever device is the corresponding role.
      vr::ETrackedControllerRole role;

      // If a role is specified (left or right controller), deviceIndex
      // will automatically be updated to the device's index.
      // However, if the Invalid role is specified, then this device tracks
      // to the deviceIndex's pose.
      int deviceIndex;

  }; // class VRTrackedObject

} // namespace dg
