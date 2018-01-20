//
//  Scene.h
//

#pragma once

#include <forward_list>
#include <memory>
#include <Window.h>
#include <Behavior.h>
#include <SceneObject.h>
#include <Skybox.h>
#include <Model.h>
#include <Light.h>

namespace dg {

  class Scene : public SceneObject {

    public:

      Scene();

      virtual void Initialize() {};
      virtual void Update();
      virtual void RenderFrame();

      void SetWindow(std::shared_ptr<Window> window) {
        this->window = window;
      }

    protected:

      // Pipeline functions for overriding in special cases.
      virtual void RenderScene(const Camera& camera) const;
      virtual void PrepareModelForDraw(
          const Model& model,
          glm::vec3 cameraPosition,
          glm::mat4x4 view,
          glm::mat4x4 projection,
          const std::forward_list<Light*>& lights) const;

      std::shared_ptr<Camera> mainCamera;
      std::vector<std::unique_ptr<Behavior>> behaviors;
      std::shared_ptr<Window> window = nullptr;
      std::unique_ptr<Skybox> skybox = nullptr;
      bool started = false;

  }; // class Scene

} // namespace dg
