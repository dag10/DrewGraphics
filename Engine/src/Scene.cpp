//
//  Scene.cpp
//

#include "dg/Scene.h"
#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <vector>
#include "dg/Camera.h"
#include "dg/Exceptions.h"
#include "dg/FrameBuffer.h"
#include "dg/Graphics.h"
#include "dg/Lights.h"
#include "dg/Model.h"
#include "dg/RasterizerState.h"
#include "dg/Skybox.h"
#include "dg/Window.h"
#include "dg/materials/ScreenQuadMaterial.h"
#include "dg/vr/VRManager.h"
#include "dg/vr/VRRenderModel.h"
#include "dg/vr/VRTrackedObject.h"

dg::Scene::Scene() : SceneObject() {}
dg::Scene::~Scene() {}

void dg::Scene::Initialize() {
  // Create default main camera.
  cameras.main = std::make_shared<Camera>();
  cameras.main->transform.translation = glm::vec3(0, 1.5, 0);
  AddChild(cameras.main);

  // Attempt to set up VR if requested. This might fail, check vr.enabled.
  if (vr.requested) {
    InitializeVR();
  }

  // Create main subrender configuration. This configuration's
  // concrete rasterizerState is the basis for other configuration's
  // potentially abstract rasterizerStates. Also, its camera is set
  // each frame instead of now in case cameras.main changes.
  subrenders.main.rasterizerState = RasterizerState::Default();
  subrenders.main.type = Subrender::Type::MonoscopicWindow;

  // Create the subrender configuration as the basis for all monoscopic
  // framebuffer subrenders. This does not apply to stereoscopic framebuffers
  // or shadowmap framebuffers.
  subrenders.framebuffer.type = Subrender::Type::MonoscopicFramebuffer;
#if defined(_DIRECTX)
  subrenders.framebuffer.rasterizerState.SetFlipRenderY(true, true);
#endif

  // Create subrender state for light shadowmap, if any. If a depth map
  // is eventually rendered, the framebuffer resource will be created only at
  // that time.
  subrenders.light.type = Subrender::Type::Shadowmap;
  subrenders.light.camera = std::make_shared<Camera>();

  // If we're set up for VR, create subrender configurations for each eye.
  // Its camera is set each frame instead of now in case cameras.vr changes.
  if (vr.enabled) {
    for (int i = 0; i < 2; i++) {
      subrenders.eyes[i].eye =
          i == 0 ? vr::EVREye::Eye_Left : vr::EVREye::Eye_Right;
      subrenders.eyes[i].type = Subrender::Type::Stereoscopic;
      subrenders.eyes[i].framebuffer =
          VRManager::Instance->GetFramebuffer(subrenders.eyes[i].eye);
    }
  }
}

void dg::Scene::InitializeVR() {
  if (vr.enabled) {
    return;
  }

  // Create container for OpenVR behaviors and tracked devices.
  auto vrContainer = std::make_shared<SceneObject>();
  Behavior::Attach(vrContainer, std::make_shared<VRManager>());

  // Try to initialize OpenVR. If that fails, fall back to non-VR mode.
  try {
    VRManager::Instance->StartOpenVR();
  } catch (const OpenVRError &e) {
    std::cerr << "Failed to initialize VR: " << e.what() << std::endl
              << std::endl;
    return;
  }

  vr.hiddenAreaMeshMaterial = std::make_shared<ScreenQuadMaterial>(
      glm::vec3(0), glm::vec2(2), glm::vec2(-1));
  vr.hiddenAreaMeshMaterial->rasterizerOverride.SetDepthFunc(
      RasterizerState::DepthFunc::ALWAYS);
  vr.hiddenAreaMeshMaterial->rasterizerOverride.SetCullMode(
      RasterizerState::CullMode::OFF);

#ifdef _OPENGL
  // Disable glfw vsync, since IVRComposer::WaitGetPoses() will wait for
  // "running start" in 90hz anyway.
  glfwSwapInterval(0);
#endif

  vr.container = vrContainer;
  AddChild(vrContainer);

  if (cameras.vr == nullptr) {
    cameras.vr = cameras.main;
    vr.container->AddChild(cameras.vr);
    Behavior::Attach(cameras.vr, std::make_shared<VRTrackedObject>(0));
  }

  vr.enabled = true;
}

void dg::Scene::Update() {
  // Traverse the scene hierarchy and update all behaviors on all objects.
  std::forward_list<SceneObject*> remainingObjects;
  remainingObjects.push_front((SceneObject*)this);
  while (!remainingObjects.empty()) {
    SceneObject *obj = remainingObjects.front();
    remainingObjects.pop_front();
    obj->UpdateBehaviors();
    for (auto child = obj->Children().begin();
         child != obj->Children().end();
         child++) {
      if (!(*child)->enabled) continue;
      remainingObjects.push_front(child->get());
    }
  }
}

void dg::Scene::ClearBuffer() {
  Graphics::Instance->ClearColor(glm::vec3(0));
}

void dg::Scene::SetupRender() {
  currentRender.rendering = true;
  ProcessSceneHierarchy();
  if (vr.enabled) {
    // Wait for "running start", and get latest poses. This is blocking.
    VRManager::Instance->ReadyToRender();
  }
}

void dg::Scene::SetupSubrender(Subrender &subrender) {
  currentRender.subrender = &subrender;

  if (subrender.type == Subrender::Type::None) {
    throw std::runtime_error("Attempted to set up invalid subrender.");
  }

  if (subrender.camera == nullptr && subrender.drawScene) {
    throw std::runtime_error("Non-custom subrenders must have a camera.");
  }

  // Set up render target.
  if (subrender.framebuffer == nullptr) {
    Graphics::Instance->SetRenderTarget(*window);
    if (subrender.camera != nullptr) {
      subrender.camera->aspectRatio = window->GetAspectRatio();
    }
  } else {
    Graphics::Instance->SetRenderTarget(*subrender.framebuffer);
    if (subrender.camera != nullptr) {
      subrender.camera->aspectRatio = subrender.framebuffer->GetAspectRatio();
    }
  }

  // Always use the main subrender's rasterizer state as the base rasterizer
  // state, and derive off of that.
  RasterizerState rasterizerState = subrenders.main.rasterizerState;
  switch (subrender.type) {
    case Subrender::Type::MonoscopicFramebuffer:
    case Subrender::Type::Shadowmap:
      rasterizerState += subrenders.framebuffer.rasterizerState;
      break;
    default:
      break;
  }
  rasterizerState += subrender.rasterizerState;
  Graphics::Instance->PushRasterizerState(rasterizerState);

  // Clear the background and depth and stencil buffers.
  ClearBuffer();

  // If we're rendering to HMD, draw the hidden area mesh for early-out of
  // pixels we won't see due to the HMD's optics.
  if (subrender.type == Subrender::Type::Stereoscopic) {
    DrawHiddenAreaMesh(subrender.eye);
  }
}

void dg::Scene::TeardownSubrender() {
  if (currentRender.subrender->type == Subrender::Type::Stereoscopic) {
    VRManager::Instance->SubmitFrame(currentRender.subrender->eye);
  }
  Graphics::Instance->PopRasterizerState();
  currentRender.subrender = nullptr;
}

void dg::Scene::TeardownRender() {
  if (vr.enabled) {
    VRManager::Instance->RenderFinished();
  }
  currentRender.models.clear();
  currentRender.lights.clear();
  currentRender.shadowCastingLight = nullptr;
  currentRender.rendering = false;
}

void dg::Scene::RenderFrame() {
  subrenders.main.camera = cameras.main;
  PreRender();
  SetupRender();
  RenderLightShadowMap();
  RenderFramebuffers();
  if (vr.enabled) {
    for (int i = 0; i < 2; i++) {
      subrenders.eyes[i].camera = cameras.vr;
      PerformSubrender(subrenders.eyes[i]);
    }
  }
  PerformSubrender(subrenders.main);
  PostProcess();
  TeardownRender();
  PostRender();
  ResourceReadback();
}

void dg::Scene::PerformSubrender(Subrender &subrender) {
  if (!currentRender.rendering) {
    throw std::runtime_error(
        "PerformSubrender() called outside the render loop.");
  }

  SetupSubrender(subrender);
  PreSubrender(subrender);
  if (subrender.drawScene) {
    DrawScene();
  } else {
    DrawCustomSubrender(subrender);
  }
  PostSubrender(subrender);
  TeardownSubrender();
}

void dg::Scene::ProcessSceneHierarchy() {
  currentRender.models.clear();
  currentRender.lights.clear();

  // Recursively cache the scene-space transforms of all SceneObjects.
  CacheSceneSpace();

  // Traverse scene tree and sort out different types of objects
  // into their own lists.
  std::deque<SceneObject*> remainingObjects;
  remainingObjects.push_front((SceneObject*)this);
  while (!remainingObjects.empty()) {
    SceneObject *obj = remainingObjects.front();
    remainingObjects.pop_front();
    for (auto child = obj->Children().begin();
         child != obj->Children().end();
         child++) {
      if (!(*child)->enabled) continue;
      remainingObjects.push_front(child->get());
      if (auto model = std::dynamic_pointer_cast<Model>(*child)) {
        currentRender.models.push_back(SortedModel(*model));
      } else if (auto light = std::dynamic_pointer_cast<Light>(*child)) {
        currentRender.lights.push_front(light.get());
      }
    }
  }

  // Compute all models' distances to camera.
  glm::vec3 cameraPos = cameras.main->CachedSceneSpace().translation;
  for (SortedModel &sortedModel : currentRender.models) {
    sortedModel.distanceToCamera = glm::distance(
        sortedModel.model->CachedSceneSpace().translation, cameraPos);
  }

  // Sort models.
  std::sort(currentRender.models.begin(), currentRender.models.end(),
       [](SortedModel &a, SortedModel &b) -> bool {
         // First short by render queue order.
         if (a.model->material->queue != b.model->material->queue) {
           return a.model->material->queue < b.model->material->queue;
         }

         // Then short by distance to camera. If render queue is
         // < Transparent, draw objects closer to camera first (for early-out
         // in fragment shader). Otherwise, draw from back to front for
         // transparency.
         if (a.model->material->queue < RenderQueue::Transparent) {
           return a.distanceToCamera < b.distanceToCamera;
         } else {
           return a.distanceToCamera > b.distanceToCamera;
         }
       });

  // Reset light shadows.
  bool foundShadowLight = false;
  currentRender.shadowCastingLight = nullptr;
  for (auto &light : currentRender.lights) {
    light->SetShadowMap(nullptr);
    if (light->GetCastShadows()) {
      if (foundShadowLight) {
        std::cerr
            << "Warning: More than one light wants to cast shadows, but only "
               "one shadow is supported at a time."
            << std::endl;
      } else {
        foundShadowLight = true;
        currentRender.shadowCastingLight = light;
      }
    }
  }
}

void dg::Scene::RenderLightShadowMap() {
  if (currentRender.shadowCastingLight == nullptr) {
    return;
  }

  switch (currentRender.shadowCastingLight->GetShaderData().type) {
    case Light::LightType::NONE:
      return;
    case Light::LightType::POINT:
      std::cerr << "Error: Shadows are not implemented for PointLight."
                << std::endl;
      return;
    case Light::LightType::SPOT:
      break;
    case Light::LightType::DIRECTIONAL:
      std::cerr << "Error: Shadows are not implemented for DirectionalLight."
                << std::endl;
      return;
  }

  if (subrenders.light.framebuffer == nullptr) {
    FrameBuffer::Options options;
    options.width = 2048;
    options.height = 2048;
    options.depthReadable = true;
    options.hasColor = false;
    options.hasStencil = false;
    subrenders.light.framebuffer = FrameBuffer::Create(options);
  }

  auto *spotlight = static_cast<SpotLight *>(currentRender.shadowCastingLight);
  auto camera = subrenders.light.camera;
  camera->transform = spotlight->CachedSceneSpace();
  camera->fov = spotlight->GetCutoff() * 2;
  camera->nearClip = 0.01f;
  camera->farClip = 100;
  currentRender.shadowCastingLight->SetLightTransform(
      camera->GetProjectionMatrix() * camera->GetViewMatrix());
  PerformSubrender(subrenders.light);
  currentRender.shadowCastingLight->SetShadowMap(
      subrenders.light.framebuffer->GetDepthTexture());
}

void dg::Scene::DrawScene() {
  assert(currentRender.subrender != nullptr);

  // Render skybox.
  if (skybox != nullptr && skybox->enabled &&
      currentRender.subrender->renderSkybox) {
    switch (currentRender.subrender->type) {
      case Subrender::Type::MonoscopicWindow:
      case Subrender::Type::MonoscopicFramebuffer:
        skybox->Draw(*currentRender.subrender->camera);
        break;
      case Subrender::Type::Stereoscopic:
        skybox->Draw(*currentRender.subrender->camera,
                     currentRender.subrender->eye);
        break;
      default:
        break;
    }
  }

  // Set up view.
  glm::mat4x4 view;
  glm::mat4x4 projection;
  switch (currentRender.subrender->type) {
    case Subrender::Type::Stereoscopic:
      view = currentRender.subrender->camera->GetViewMatrix(
          currentRender.subrender->eye);
      projection = currentRender.subrender->camera->GetProjectionMatrix(
          currentRender.subrender->eye);
      break;
    default:
      view = currentRender.subrender->camera->GetViewMatrix();
      projection = currentRender.subrender->camera->GetProjectionMatrix();
      break;
  }

  // Prepare light data.
  Light::ShaderData lightArray[Light::MAX_LIGHTS];
  int lightIdx = 0;
  for (auto &light : currentRender.lights) {
    if (lightIdx >= Light::MAX_LIGHTS) {
      break;
    }
    lightArray[lightIdx++] = (*light).GetShaderData();
  }

  // Render models.
  glm::vec4 cameraPos_h = glm::inverse(view) * glm::vec4(0, 0, 0, 1);
  glm::vec3 cameraPos = glm::vec3(cameraPos_h) / cameraPos_h.w;
  for (auto &currentModel : currentRender.models) {
    std::shared_ptr<Material> material = currentRender.subrender->material;
    if (material == nullptr) {
      material = currentModel.model->material;
    }
    if (!(currentModel.model->layer & currentRender.subrender->layerMask)) {
      continue;
    }
    Model::DrawContext context;
    context.view = view;
    context.projection = projection;
    context.cameraPos = &cameraPos;
    context.lights = &lightArray;
    if (currentRender.shadowCastingLight != nullptr) {
      auto texture = currentRender.shadowCastingLight->GetShadowMap();
      if (texture != nullptr) {
        context.shadowMap = texture;
      }
    }
    (*currentModel.model).Draw(context, currentRender.subrender->material);
  }
}

bool dg::Scene::AutomaticWindowTitle() const {
  return true;
}

void dg::Scene::DrawHiddenAreaMesh(vr::EVREye eye) {
  auto mesh = VRManager::Instance->GetHiddenAreaMesh(eye);
  if (mesh != nullptr && mesh->IsDrawable()) {
    Graphics::Instance->PushRasterizerState(
        vr.hiddenAreaMeshMaterial->rasterizerOverride);
    vr.hiddenAreaMeshMaterial->Use();
    mesh->Draw();
    Graphics::Instance->PopRasterizerState();
  }
}
