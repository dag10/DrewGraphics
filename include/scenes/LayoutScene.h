//
//  scenes/LayoutScene.h
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

  class LayoutScene : public Scene {

    public:

      static std::unique_ptr<LayoutScene> Make();

      LayoutScene();

      virtual void Initialize();

  }; // class LayoutScene

} // namespace dg
