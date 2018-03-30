//
//  Scene.h
//

#pragma once

#include <openvr.h>
#include <deque>
#include <forward_list>
#include <memory>
#include <vector>
#include "dg/FrameBuffer.h"
#include "dg/Lights.h"
#include "dg/RasterizerState.h"
#include "dg/SceneObject.h"

namespace dg {

  class Camera;
  class Model;
  class Window;
  class Skybox;
  class Light;
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

      struct CurrentModel {
        Model *model;
        float distanceToCamera = 0;

        CurrentModel(Model &model) {
          this->model = &model;
        }
      };

      // Pipeline functions for overriding in special cases.
      virtual void RenderFrame(vr::EVREye eye);
      virtual void RenderFrameBuffer(FrameBuffer &frameBuffer,
                                     const Camera &camera);
      virtual void ProcessSceneHierarchy();
      virtual void RenderLightShadowMap();
      virtual void DrawScene(
          const Camera& camera, bool renderForVR = false,
          vr::EVREye eye = vr::EVREye::Eye_Left);
      virtual void PrepareModelForDraw(
          const Model& model, glm::vec3 cameraPosition, glm::mat4x4 view,
          glm::mat4x4 projection,
          const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) const;
      virtual void ClearBuffer();
      virtual void DrawHiddenAreaMesh(vr::EVREye eye);

      RasterizerState defaultRasterizerState;
      RasterizerState renderingToFrameBufferRasterizerState;

      std::shared_ptr<Camera> mainCamera;
      std::shared_ptr<Window> window = nullptr;
      std::shared_ptr<Skybox> skybox = nullptr;

      // Virtual reality
      bool enableVR = false;
      std::shared_ptr<SceneObject> vrContainer;
      std::shared_ptr<ScreenQuadMaterial> hiddenAreaMeshMaterial = nullptr;

      // Scene hierarchy for current frame.
      std::vector<CurrentModel> currentModels;
      std::deque<Light *> currentLights;
      Light *shadowCastingLight = nullptr;
      std::shared_ptr<FrameBuffer> shadowFrameBuffer = nullptr;

  }; // class BaseScene

  using Scene = BaseScene;

} // namespace dg
