//
//  scenes/ShadowScene.h
//

#pragma once

#include <memory>
#include "dg/Scene.h"

namespace dg {

  class FrameBuffer;
  class ScreenQuadMaterial;
  class SpotLight;

  class ShadowScene : public Scene {

    public:

      static std::unique_ptr<ShadowScene> Make();

      ShadowScene();

      virtual void Initialize();
      virtual void Update();

    private:

      enum class State {
        Spotlight,
        RenderLight,
        RenderDepth,
        Shadow,
        Feather,
        Robot,
      };

      virtual void RenderFrame();
      virtual void PrepareModelForDraw(
          const Model& model,
          glm::vec3 cameraPosition,
          glm::mat4x4 view,
          glm::mat4x4 projection,
          const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) const;
      void SetState(State state);
      void CreateRobotScene();

      std::shared_ptr<SceneObject> lightScene = nullptr;
      std::shared_ptr<SceneObject> robotScene = nullptr;

      std::shared_ptr<Model> cube = nullptr;
      std::shared_ptr<FrameBuffer> framebuffer = nullptr;
      std::shared_ptr<SpotLight> spotlight = nullptr;
      std::shared_ptr<SpotLight> robotSpotlight = nullptr;
      std::shared_ptr<ScreenQuadMaterial> quadMaterial = nullptr;
      State state;

      // TODO: Refactor this
      glm::mat4 lightTransform = glm::mat4(0);

      // Robot joints.
      std::shared_ptr<SceneObject> robot;
      std::shared_ptr<SceneObject> leftShoulder;
      std::shared_ptr<SceneObject> rightShoulder;
      std::shared_ptr<SceneObject> leftElbow;
      std::shared_ptr<SceneObject> rightElbow;
      std::shared_ptr<SceneObject> neck;
      std::shared_ptr<SceneObject> eyes;

      // Robot state.
      float nextBlink = -1;
      float endOfBlink = -1;

      std::shared_ptr<SceneObject> lightContainer;

  }; // class ShadowScene

} // namespace dg
