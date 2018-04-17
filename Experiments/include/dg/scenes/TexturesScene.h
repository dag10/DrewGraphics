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

      class LayerMask : public Scene::LayerMask {
        public:
          // Layer for models which display the texture of a framebuffer.
          // Used to hide said models when rendering to said framebuffer.
          static inline LayerMask UsesFramebuffer() { return Ord(1); }
        protected:
          LayerMask(Scene::LayerMask mask) : Scene::LayerMask(mask) {}
        private:
          static inline LayerMask Ord(uint64_t idx) {
            return LayerMask(Scene::LayerMask::Ord(HighestIndex + idx));
          }
      };

      virtual void RenderFramebuffers();

      Subrender quadSubrender;
      std::shared_ptr<Camera> virtualCamera;

      bool flyingMainCamera = true;

  }; // class TexturesScene

} // namespace dg
