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

namespace dg {

  class TutorialScene : public Scene {

    public:

      static std::unique_ptr<TutorialScene> Make();

      TutorialScene();

      virtual void Initialize();
      virtual void Update();
      virtual void Render();

    private:

  }; // class TutorialScene

} // namespace dg
