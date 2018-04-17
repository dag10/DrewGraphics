//
//  scenes/TexturesScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class SceneObject;
  class Camera;

  class TexturesScene : public Scene {

    public:

      static std::unique_ptr<TexturesScene> Make();

      TexturesScene();

      virtual void Initialize();
      virtual void Update();

    private:

      virtual void RenderFramebuffers();

	  Subrender quadSubrender;
      std::shared_ptr<Camera> virtualCamera;

      bool flyingMainCamera = true;

  }; // class TexturesScene

} // namespace dg
