//
//  vr/VRManager.h
//

#pragma once

#include "dg/Transform.h"
#include "dg/Behavior.h"
#include "dg/FrameBuffer.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <forward_list>
#include <openvr.h>

namespace dg {

  class VRTrackedObject;
  class Mesh;

  class VRManager : public Behavior {

    public:

      class RenderModelInfo {
      public:
        uint32_t renderModelIndex;
        std::shared_ptr<Mesh> mesh = nullptr;
      };

      static VRManager *Instance;

      vr::IVRSystem *vrSystem = nullptr;
      vr::IVRCompositor *vrCompositor = nullptr;
      vr::IVRRenderModels *vrRenderModels = nullptr;

      VRManager() = default;
      virtual ~VRManager();

      virtual void Initialize();

      // To be called by the scene when the update is finished and
      // it's ready to render. This will block until OpenVR's "running start".
      void ReadyToRender();

      // To be called by the scene once the render is finished.
      void RenderFinished();

      // These methods should only be called from
      // VRTrackedObject::Initialize().
      void RegisterTrackedObject(VRTrackedObject *object);
      void DeregisterTrackedObject(VRTrackedObject *object);

      std::shared_ptr<FrameBuffer> GetFramebuffer(vr::EVREye eye) const;
      void SubmitFrame(vr::EVREye eye);

      std::shared_ptr<Mesh> GetRenderModelMesh(const std::string& name);

      std::shared_ptr<Mesh> GetHiddenAreaMesh(vr::EVREye eye);

    private:

      void StartOpenVR();
      void CreateFramebuffers();
      void UpdatePoses();
      void PopulateRenderModelList();

      int leftControllerIndex = -1;
      int rightControllerIndex = -1;

      std::shared_ptr<FrameBuffer> leftFramebuffer;
      std::shared_ptr<FrameBuffer> rightFramebuffer;

      std::shared_ptr<Mesh> leftHiddenAreaMesh = nullptr;
      std::shared_ptr<Mesh> rightHiddenAreaMesh = nullptr;

      std::vector<vr::TrackedDevicePose_t> poses
        = std::vector<vr::TrackedDevicePose_t>(vr::k_unMaxTrackedDeviceCount);
      std::vector<vr::TrackedDevicePose_t> nextPoses
        = std::vector<vr::TrackedDevicePose_t>(vr::k_unMaxTrackedDeviceCount);
      std::forward_list<VRTrackedObject*> trackedObjects;

      std::unordered_map<std::string, std::shared_ptr<RenderModelInfo>> renderModels;

  }; // class VRManager

} // namespace dg
