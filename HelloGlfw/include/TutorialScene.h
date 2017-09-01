//
//  TutorialScene.h
//

#pragma once

#include "Scene.h"
#include "Window.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include <memory>

namespace dg {

  class TutorialScene : public Scene {

    public:

      virtual void Initialize();
      virtual void Update();
      virtual void Render(dg::Window& window);

    private:

      std::shared_ptr<dg::Mesh> cube;
      dg::Camera camera;
      dg::Shader shader;
      dg::Texture containerTexture;
      dg::Texture awesomeFaceTexture;

  }; // class TutorialScene

} // namespace dg
