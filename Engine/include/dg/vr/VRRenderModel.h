//
//  vr/VRRenderModel.h
//

#pragma once

#include <openvr.h>
#include <memory>
#include <string>
#include "dg/Behavior.h"

namespace dg {

  class Model;
  class VRTrackedObject;

  class VRRenderModel : public Behavior {

    public:

      virtual void Start();
      virtual void Update();

    private:

      void DestroyRenderModel();
      void LoadRenderModel(const std::string& modelName);

      std::weak_ptr<VRTrackedObject> trackedObject;
      int deviceIndex = -1;
      std::string currentModelName;
      std::weak_ptr<Model> model;
      double nextLoadRetryTime = 0;

  }; // class VRRenderModel

} // namespace dg
