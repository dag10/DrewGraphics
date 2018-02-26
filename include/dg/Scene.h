//
//  Scene.h
//

#pragma once

#include "dg/Lights.h"
#include "dg/SceneObject.h"
#include <forward_list>
#include <memory>

namespace dg {

  class Camera;
  class Model;
  class Window;
  class Skybox;
  class Light;
  class FrameBuffer;
  class ScreenQuadMaterial;

  class BaseScene : public SceneObject {

    public:

      BaseScene();
      virtual ~BaseScene();

      virtual void Initialize();
      virtual void Update();
      virtual void RenderFrame();

      virtual bool AutomaticWindowTitle() const;

      void SetWindow(std::shared_ptr<Window> window) {
        this->window = window;
      }

    protected:

      // Pipeline functions for overriding in special cases.
      virtual void DrawScene(const Camera& camera);
      virtual void PrepareModelForDraw(
          const Model& model, glm::vec3 cameraPosition, glm::mat4x4 view,
          glm::mat4x4 projection,
          const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) const;
      virtual void ClearBuffer();
      virtual void ConfigureBuffer() = 0;

      std::shared_ptr<Camera> mainCamera;
      std::shared_ptr<Window> window = nullptr;
      std::shared_ptr<Skybox> skybox = nullptr;

  }; // class BaseScene


  class DirectXScene : public BaseScene {

    protected:

     virtual void ConfigureBuffer();

  }; // class DirectXScene

  using Scene = DirectXScene;


} // namespace dg