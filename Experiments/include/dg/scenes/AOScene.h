//
//  scenes/AOScene.h
//

#pragma once

#include <memory>
#include <vector>
#include "dg/Scene.h"

namespace dg {

  class Model;

  class AOScene : public Scene {

    public:

      static std::unique_ptr<AOScene> Make();

      AOScene();

      virtual void Initialize();
      virtual void Update();

    private:

      enum class OverlayState {
        None,
        GBuffer,
      };

      void InitializeSubrenders();
      void CreateGBuffer();

      virtual void RenderFramebuffers();
      virtual void PreRender();

      OverlayState overlayState = OverlayState::GBuffer;

      Subrender geometrySubrender;

      std::vector<std::shared_ptr<Model>> overlayQuads;

  }; // class AOScene

} // namespace dg
