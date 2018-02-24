//
//  Scene.h
//

#pragma once

#include <Lights.h>
#include <SceneObject.h>
#include <openvr.h>
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
      virtual void RenderFrame(vr::EVREye eye);
      virtual void DrawScene(
          const Camera& camera, bool renderForVR = false,
          vr::EVREye eye = vr::EVREye::Eye_Left);
      virtual void PrepareModelForDraw(
          const Model& model, glm::vec3 cameraPosition, glm::mat4x4 view,
          glm::mat4x4 projection,
          const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) const;
      virtual void ClearBuffer();
      virtual void DrawHiddenAreaMesh(vr::EVREye eye) = 0;
      virtual void ConfigureBuffer() = 0;

      std::shared_ptr<Camera> mainCamera;
      std::shared_ptr<Window> window = nullptr;
      std::shared_ptr<Skybox> skybox = nullptr;

      // Virtual reality
      bool enableVR = false;
      std::shared_ptr<SceneObject> vrContainer;
      std::shared_ptr<ScreenQuadMaterial> hiddenAreaMeshMaterial = nullptr;

  }; // class BaseScene

#if defined(_OPENGL)

  class OpenGLScene : public BaseScene {

    protected:

      virtual void DrawHiddenAreaMesh(vr::EVREye eye);
      virtual void ConfigureBuffer();

  }; // class OpenGLScene

  using Scene = OpenGLScene;

#elif defined(_DIRECTX)

  class DirectXScene : public BaseScene {

    protected:

     virtual void DrawHiddenAreaMesh(vr::EVREye eye);
     virtual void ConfigureBuffer();

  }; // class DirectXScene

  using Scene = DirectXScene;

#endif

} // namespace dg
