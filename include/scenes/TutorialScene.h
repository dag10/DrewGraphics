//
//  scenes/TutorialScene.h
//

#pragma once

#include <memory>
#include <Scene.h>
#include <Window.h>
#include <Camera.h>
#include <Shader.h>
#include <Texture.h>
#include <Mesh.h>
#include <Model.h>
#include <Skybox.h>
#include <FrameBuffer.h>

namespace dg {

  class TutorialScene : public Scene {

    public:

      static std::unique_ptr<TutorialScene> Make();

      TutorialScene();

      virtual void Initialize();
      virtual void Update();
      virtual void RenderFrame();

    private:

      std::shared_ptr<FrameBuffer> framebuffer;
      std::shared_ptr<Model> renderQuad;
      std::shared_ptr<Camera> virtualCamera;

  }; // class TutorialScene

} // namespace dg
