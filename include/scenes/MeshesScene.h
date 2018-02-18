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
      static std::unique_ptr<MeshesScene> MakeVR();

      virtual void Initialize();
      virtual void Update();

    private:

      MeshesScene(bool enableVR);

      std::shared_ptr<Model> spinningHelix;
      std::shared_ptr<Model> spinningTorus;

  }; // class MeshesScene

} // namespace dg
