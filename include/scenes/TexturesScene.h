//
//  scenes/TexturesScene.h
//

#pragma once

#include <memory>
#include <Scene.h>

namespace dg {

  class FrameBuffer;
  class SceneObject;
  class Camera;

  class TexturesScene : public Scene {

    public:

      static std::unique_ptr<TexturesScene> Make();

      TexturesScene();

      virtual void Initialize();
      virtual void Update();
      virtual void RenderFrame();

    private:

      std::shared_ptr<FrameBuffer> framebuffer;
      std::shared_ptr<SceneObject> renderQuads;
      std::shared_ptr<SceneObject> dummyRenderQuads;
      std::shared_ptr<Camera> virtualCamera;

      bool flyingMainCamera = true;

  }; // class TexturesScene

} // namespace dg
