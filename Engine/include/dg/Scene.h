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

  // Base class for a scene, which is the highest level of a front-end
  // experience for the engine.
  //
  // Ideally, a scene would not have any rendering logic, and be mostly
  // game logic and scene structure. However, for my purposes of easy graphics
  // experimentation, I've implemented rendering at the scene level, and
  // provided virtual functions for Scene extensions to override parts of the
  // rendering pipeline.
  //
  // For the sake of terminology, I've created two concepts: a "render" and
  // "subrender". A render is the once-per-frame process for rendering a frame.
  // A Subrender is a process which will ultimately call DrawScene() once, and
  // there might be multiple Subrenders per frame. Usually to different render
  // targets. Some example subrenders are rendering a light's shadowmap,
  // rendering each eye for VR, rendering a to a texture for use in the scene,
  // etc.
  //
  // Renders and Subrenders each have Setup-() and Teardown-() functions, which
  // are internal to Scene only. They also have Pre-() and Post-() functions
  // which are empty by default and intended to be overridden by extending
  // Scenes which need to do additional stuff.
  //
  // RenderFrame() is the external entrypoint for rendering, called once per
  // frame. Within RenderFrame(), this is the high-level flow, represented
  // as pseudo code.
  //
  // ------------ Pseudocode ----------------- | ----------- Notes ------------
  //                                           | (No notes == shouldn't extend)
  // RenderFrame() {                           |
  //   PreRender()                             | Virtual, empty by default.
  //                                           | This is the last change for a
  //                                           | scene to modify that frame's
  //                                           | scene hierarchy.
  //                                           |
  //   SetupRender()                           | Will process scene hierarchy,
  //                                           | populating the currentRender
  //                                           | struct.
  //                                           |
  //   if (shadow-producing light exists) {    |
  //     SetupSubrender(Type::Shadowmap)       | Sets framebuffer, also calls
  //                                           | ClearBuffer().
  //     PreSubrender()                        | Virtual, empty by default.
  //     DrawScene()                           |
  //     PostSubrender()                       | Virtual, empty by default.
  //     TeardownSubrender()                   |
  //   }                                       |
  //                                           |
  //   RenderFramebuffers()                    | Virtual, empty by default.
  //                                           |
  //   if (rendering VR) {                     |
  //                                           |
  //     SetupSubrender(                       | Sets framebuffer, also calls
  //         Type::Stereoscopic, left)         | ClearBuffer().
  //     DrawHiddenAreaMesh()                  |
  //     PreSubrender()                        | Virtual, empty by default.
  //     DrawScene()                           |
  //     PostSubrender()                       | Virtual, empty by default.
  //     TeardownSubrender()                   |
  //                                           |
  //     SetupSubrender(                       | Sets framebuffer, also calls
  //         Type::Stereoscopic, right)        | ClearBuffer().
  //     DrawHiddenAreaMesh()                  |
  //     PreSubrender()                        | Virtual, empty by default.
  //     DrawScene()                           |
  //     PostSubrender()                       | Virtual, empty by default.
  //     TeardownSubrender()                   |
  //                                           |
  //   }                                       |
  //                                           |
  //   SetupSubrender(Type::MonoscopicWindow)  | Sets framebuffer, also calls
  //                                           | ClearBuffer().
  //   PreSubrender()                          | Virtual, empty by default.
  //   DrawScene()                             |
  //   PostSubrender()                         | Virtual, empty by default.
  //   TeardownSubrender()                     |
  //                                           |
  //   RenderOverlays()                        | Virtual, empty by default.
  //                                           |
  //   TeardownRender()                        |
  //   PostRender()                            | Virtual, empty by default.
  // }
  //
  class Scene : public SceneObject {

    public:

      // Scenes may be created without any intent to run them. Do not perform
      // logic in the constructor.
      Scene();

      virtual ~Scene();

      // Called once per scene lifetime, in the very beginning. Initialize
      // variables here, load GPU resources, and construct the scene hierarchy.
      virtual void Initialize();

      // Called once per frame, before rendering. Perform game logic here.
      virtual void Update();

      // Called once per frame, after updating. See large class comment above
      // to understand the render control flow.
      virtual void RenderFrame();

      // Returns true if this scene does not modify the window's title, and
      // would like the engine to automatically update it instead.
      virtual bool AutomaticWindowTitle() const;

      // Called by the Engine to set the current window, called before
      // Initialize().
      void SetWindow(std::shared_ptr<Window> window) {
        this->window = window;
      }

    protected:

      // Pairing of a model that'll be rendered this frame with its distance
      // to the camera, used for sorting models by distance.
      struct SortedModel {
        Model *model;
        float distanceToCamera = 0;
        SortedModel(Model &model) { this->model = &model; }
      };

      // Configuration for a subrender. A subrender is a process within a
      // single frame's render that will walk the scene hierarchy and draw the
      // scene once to some render target.
      struct Subrender {

        enum class Type {
          // Unknown state, for catching out-of-order mistakes.
          None,

          // Rendering to window.
          MonoscopicWindow,

          // Rendering to single framebuffer.
          MonoscopicFramebuffer,

          // Rendering to left or right eye of VR HMD.
          Stereoscopic,

          // Rendering depths for a light map.
          Shadowmap,
        };

        Type type = Type::None;
        RasterizerState rasterizerState;
        std::shared_ptr<FrameBuffer> framebuffer = nullptr;
        std::shared_ptr<Camera> camera = nullptr;
        vr::EVREye eye;
        bool renderSkybox = true;
      }; // struct Subrender

      // Hook called before any rendering work begins. This is a child scene's
      // last change to modify the scene hierarchy before it's walked.
      virtual void PreRender() {};

      // Hook called after the buffer is cleared and just before the scene is
      // drawn for a subrender.
      virtual void PreSubrender(const Subrender &subrender) {};

      // Hook called just after the scene is drawn for a subrender.
      // This is the last hook called before stereoscopic renders are submitted
      // to OpenVR for display on the HMD, so render any VR post effects here.
      virtual void PostSubrender(const Subrender &subrender) {};

      // Hook called after all rendering is finished. Subrender calls cannot be
      // called during this time.
      virtual void PostRender() {};

      // Hook called after shadowmap has been rendered, but before the scene
      // has been rendered. If a virtual camera needs to be rendered for this
      // frame, render it here.
      virtual void RenderFramebuffers() {};

      // Called after the main scene has been rendered to the window, but before
      // this frame's render is unable to create any more subrenders. Render
      // post-processing effects and screen-space overlays here.
      virtual void RenderOverlays() {};

      // Clears the backbuffer, called once per subrender.
      // Override this to change background color.
      virtual void ClearBuffer();

      // Manually render a subrender.
      // Mutable because it will update the Camera's aspect ratio to match
      // that of the supplied render target.
      void PerformSubrender(Subrender &subrender);

      // The Skybox to render, or nullptr if no skybox is desired.
      std::shared_ptr<Skybox> skybox = nullptr;

      // Reference to the current window.
      std::shared_ptr<Window> window = nullptr;

      // Common subrender configurations.
      struct subrenders {
        Subrender main;
        Subrender framebuffer;
        Subrender light;
        Subrender eyes[2];
      } subrenders;

      // Cameras.
      struct {
        // Main camera.
        std::shared_ptr<Camera> main = nullptr;

        // Camera used for VR rendering. Usually points to the same camera as
        // the main camera above, but might be different if a scene has a
        // "spectator" view for the desktop window.
        std::shared_ptr<Camera> vr = nullptr;
      } cameras;

      // Virtual reality.
      struct {
        // True if this scene wants to attempt to use VR.
        bool requested = false;

        // True if the scene has successfully enabled VR.
        bool enabled = false;

        // Container of the VR play space in the scene hierarchy. The HMD
        // and controllers are all direct children of this container.
        std::shared_ptr<SceneObject> container;

        // Material used for rendering the HMD's hidden area mesh, used for
        // early-out of pixels that won't be visible through HMD's optics.
        std::shared_ptr<ScreenQuadMaterial> hiddenAreaMeshMaterial = nullptr;

      } vr;

      // Information pertaining to the current scene render.
      struct {

        // Flag to prevent out-of-order calls to PerformSubrender().
        bool rendering = false;

        // Current subrender type.
        const Subrender *subrender = nullptr;

        // Models in scene hierarchy for current frame.
        std::vector<SortedModel> models;

        // Lights in scene hierarchy for current frame.
        std::deque<Light *> lights;

        // Pointer to the light currently casting a shadow, if any.
        Light *shadowCastingLight = nullptr;

      } currentRender;

    private:

      void SetupRender();
      void SetupSubrender(Subrender &subrender);
      void TeardownSubrender();
      void TeardownRender();
      void PrepareModelForDraw(
          const Model &model, glm::vec3 cameraPosition, glm::mat4x4 view,
          glm::mat4x4 projection,
          const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) const;
      void DrawScene();
      void ProcessSceneHierarchy();
      void RenderLightShadowMap();
      void InitializeVR();
      void DrawHiddenAreaMesh(vr::EVREye eye);

  }; // class Scene

} // namespace dg
