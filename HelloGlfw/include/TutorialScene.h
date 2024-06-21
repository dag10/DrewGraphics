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
#include "Model.h"
#include <memory>

namespace dg {

  class TutorialScene : public Scene {

    public:

      static std::unique_ptr<TutorialScene> Make();

      virtual void Initialize();
      virtual void Update();
      virtual void Render();

    private:

      std::vector<Model> models;
      Transform xfLight;
      Camera camera;
      std::shared_ptr<Shader> solidColorShader;

  }; // class TutorialScene

} // namespace dg