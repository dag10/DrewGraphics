//
//  Scene.h
//

#pragma once

#include <forward_list>
#include <memory>
#include <Window.h>
#include <SceneObject.h>
#include <Skybox.h>
#include <Model.h>
#include <Light.h>
#include <FrameBuffer.h>
#include <materials/ScreenQuadMaterial.h>

namespace dg {

  class Scene : public SceneObject {

    public:

      Scene();

      virtual void Initialize();
      virtual void Update();
      virtual void RenderFrame();

      virtual bool AutomaticWindowTitle() const;

      void SetWindow(std::shared_ptr<Window> window) {
        this->window = window;
      }

      std::shared_ptr<Camera> GetMainCamera() const;

    protected:

      // Pipeline functions for overriding in special cases.
      virtual void RenderFrame(vr::EVREye eye);
      virtual void RenderScene(
        const Camera& camera, bool renderForVR = false,
        vr::EVREye eye = vr::EVREye::Eye_Left);
      virtual void PrepareModelForDraw(
          const Model& model,
          glm::vec3 cameraPosition,
          glm::mat4x4 view,
          glm::mat4x4 projection,
          const std::forward_list<Light*>& lights) const;
      virtual void ClearBuffer();
      virtual void DrawHiddenAreaMesh(vr::EVREye eye);
      virtual void ConfigureBuffer();

      std::shared_ptr<Camera> mainCamera;
      std::shared_ptr<Window> window = nullptr;
      std::unique_ptr<Skybox> skybox = nullptr;

      // Virtual reality
      bool enableVR = false;
      std::shared_ptr<SceneObject> vrContainer;

  private:

      std::unique_ptr<ScreenQuadMaterial> hiddenAreaMeshMaterial = nullptr;

  }; // class Scene

} // namespace dg