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

namespace dg {

  class TutorialScene : public Scene {

    public:

      static std::unique_ptr<TutorialScene> Make();

      TutorialScene();

      virtual void Initialize();

  }; // class TutorialScene

} // namespace dg
