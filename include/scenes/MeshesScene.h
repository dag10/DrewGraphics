//
//  scenes/MeshesScene.h
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

  class MeshesScene : public Scene {

    public:

      static std::unique_ptr<MeshesScene> Make();

      MeshesScene();

      virtual void Initialize();

  }; // class MeshesScene

} // namespace dg
