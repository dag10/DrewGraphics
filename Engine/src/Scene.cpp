//
//  Scene.cpp
//

#include "dg/Scene.h"
#include <algorithm>
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
#include "dg/vr/VRTrackedObject.h"

dg::Scene::Scene() : SceneObject() {}
dg::Scene::~Scene() {}

void dg::Scene::Initialize() {
  defaultRasterizerState = RasterizerState::Default();
#if defined(_DIRECTX)
  renderingToFrameBufferRasterizerState.SetFlipRenderY(true, true);
#endif

  if (enableVR) {
    // Create container for OpenVR behaviors and tracked devices.
    auto vrContainer = std::make_shared<SceneObject>();
    Behavior::Attach(vrContainer, std::make_shared<VRManager>());

    // Try to initialize OpenVR. If that fails, fall back to non-VR mode.
    try {
      VRManager::Instance->StartOpenVR();
    } catch (const OpenVRError &e) {
      std::cerr << "Failed to initialize VR: " << e.what() << std::endl
                << std::endl;
      enableVR = false;
      goto vrFailed;
    }

    this->vrContainer = vrContainer;
    AddChild(vrContainer);

    hiddenAreaMeshMaterial = std::make_shared<ScreenQuadMaterial>(
      glm::vec3(0), glm::vec2(2), glm::vec2(-1));
    hiddenAreaMeshMaterial->rasterizerOverride.SetDepthFunc(
        RasterizerState::DepthFunc::ALWAYS);
    hiddenAreaMeshMaterial->rasterizerOverride.SetCullMode(
        RasterizerState::CullMode::OFF);

#ifdef _OPENGL
    // Disable glfw vsync, since IVRComposer::WaitGetPoses() will wait for
    // "running start" in 90hz anyway.
    glfwSwapInterval(0);
#endif
  }
vrFailed:

  // Create camera.
  mainCamera = std::make_shared<Camera>();
  mainCamera->transform.translation = glm::vec3(0, 1.5, 0);
  if (enableVR) {
    vrContainer->AddChild(mainCamera);
    Behavior::Attach(
      mainCamera,
      std::make_shared<VRTrackedObject>(0));
  } else {
    AddChild(mainCamera);
  }
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

void dg::Scene::RenderFrame() {
  Graphics::Instance->PushRasterizerState(defaultRasterizerState);

  ProcessSceneHierarchy();
  RenderLightShadowMap();

  if (enableVR) {
    // Wait for "running start", and get latest poses.
    VRManager::Instance->ReadyToRender();

    // Render left and right eyes for VR.
    RenderFrame(vr::EVREye::Eye_Left);
    RenderFrame(vr::EVREye::Eye_Right);
  }

  // TODO: If VR, just render a quad of the left eye instead.
  Graphics::Instance->SetRenderTarget(*window);
  ClearBuffer();
  mainCamera->aspectRatio = window->GetAspectRatio();
  DrawScene(*mainCamera);

  if (enableVR) {
    VRManager::Instance->RenderFinished();
  }

  Graphics::Instance->PopRasterizerState();
}

void dg::Scene::RenderFrame(vr::EVREye eye) {
  std::shared_ptr<FrameBuffer> framebuffer =
    VRManager::Instance->GetFramebuffer(eye);
  Graphics::Instance->SetRenderTarget(*framebuffer);
  ClearBuffer();
  DrawHiddenAreaMesh(eye);
  DrawScene(*mainCamera, true, eye);
  VRManager::Instance->SubmitFrame(eye);
  Graphics::Instance->SetRenderTarget(*window);
}

void dg::Scene::RenderFrameBuffer(FrameBuffer &frameBuffer,
                                      const Camera &camera) {
  Graphics::Instance->SetRenderTarget(frameBuffer);
  Graphics::Instance->PushRasterizerState(defaultRasterizerState);
  Graphics::Instance->PushRasterizerState(
      renderingToFrameBufferRasterizerState);
  ClearBuffer();
  DrawScene(camera);
  Graphics::Instance->PopRasterizerState();
  Graphics::Instance->PopRasterizerState();
  Graphics::Instance->SetRenderTarget(*window);
}

void dg::Scene::ProcessSceneHierarchy() {
  currentModels.clear();
  currentLights.clear();

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
        currentModels.push_back(CurrentModel(*model));
      } else if (auto light = std::dynamic_pointer_cast<Light>(*child)) {
        currentLights.push_front(light.get());
      }
    }
  }

  // Compute all models' distances to camera.
  glm::vec3 cameraPos = mainCamera->CachedSceneSpace().translation;
  for (CurrentModel &currentModel : currentModels) {
    currentModel.distanceToCamera = glm::distance(
        currentModel.model->CachedSceneSpace().translation, cameraPos);
  }

  // Sort models.
  std::sort(currentModels.begin(), currentModels.end(),
       [](CurrentModel &a, CurrentModel &b) -> bool {
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
  shadowCastingLight = nullptr;
  for (auto &light : currentLights) {
    light->SetShadowMap(nullptr);
    if (light->GetCastShadows()) {
      if (foundShadowLight) {
        std::cerr
            << "Warning: More than one light wants to cast shadows, but only "
               "one shadow is supported at a time."
            << std::endl;
      } else {
        foundShadowLight = true;
        shadowCastingLight = light;
      }
    }
  }
}

void dg::Scene::RenderLightShadowMap() {
  if (shadowCastingLight == nullptr) {
    return;
  }

  switch (shadowCastingLight->GetShaderData().type) {
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

  if (shadowFrameBuffer == nullptr) {
    shadowFrameBuffer = FrameBuffer::Create(2048, 2048, true, false, false);
  }

  SpotLight *spotlight = static_cast<SpotLight*>(shadowCastingLight);

  Camera lightCamera;
  lightCamera.transform = spotlight->CachedSceneSpace();
  lightCamera.fov = spotlight->GetCutoff() * 2;
  lightCamera.nearClip = 0.01f;
  lightCamera.farClip = 100;
  shadowCastingLight->SetLightTransform(lightCamera.GetProjectionMatrix() *
                                        lightCamera.GetViewMatrix());
  Graphics::Instance->SetRenderTarget(*shadowFrameBuffer);
  Graphics::Instance->PushRasterizerState(defaultRasterizerState);
  Graphics::Instance->PushRasterizerState(renderingToFrameBufferRasterizerState);
  Graphics::Instance->ClearDepthStencil(true, false);
  DrawScene(lightCamera);
  Graphics::Instance->PopRasterizerState();
  Graphics::Instance->PopRasterizerState();
  Graphics::Instance->SetRenderTarget(*window);
  shadowCastingLight->SetShadowMap(shadowFrameBuffer->GetDepthTexture());
}

void dg::Scene::DrawScene(
  const Camera& camera, bool renderForVR, vr::EVREye eye) {

  // Render skybox.
  if (skybox != nullptr && skybox->enabled) {
    if (enableVR && renderForVR) {
      skybox->Draw(camera, eye);
    } else {
      skybox->Draw(camera);
    }
  }

  // Set up view.
  glm::mat4x4 view;
  glm::mat4x4 projection;
  if (renderForVR && enableVR) {
    view = camera.GetViewMatrix(eye);
    projection = camera.GetProjectionMatrix(eye);
  } else {
    view = camera.GetViewMatrix();
    projection = camera.GetProjectionMatrix();
  }

  // Prepare light data.
  Light::ShaderData lightArray[Light::MAX_LIGHTS];
  int lightIdx = 0;
  for (auto &light : currentLights) {
    if (lightIdx >= Light::MAX_LIGHTS) {
      break;
    }
    lightArray[lightIdx++] = (*light).GetShaderData();
  }

  // Render models.
  glm::vec4 cameraPos_h = glm::inverse(view) * glm::vec4(0, 0, 0, 1);
  glm::vec3 cameraPos = glm::vec3(cameraPos_h) / cameraPos_h.w;
  for (auto &currentModel : currentModels) {
    PrepareModelForDraw(
        *currentModel.model, cameraPos, view, projection, lightArray);
    (*currentModel.model).Draw(view, projection);
  }
}

void dg::Scene::PrepareModelForDraw(
    const Model& model, glm::vec3 cameraPosition, glm::mat4x4 view,
    glm::mat4x4 projection,
    const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) const {

  model.material->SendCameraPosition(cameraPosition);
  model.material->SendLights(lights);
  if (shadowCastingLight != nullptr) {
    auto texture = shadowCastingLight->GetShadowMap();
    if (texture != nullptr) {
      model.material->SendShadowMap(texture);
    }
  }
}

bool dg::Scene::AutomaticWindowTitle() const {
  return true;
}

void dg::Scene::DrawHiddenAreaMesh(vr::EVREye eye) {
  auto mesh = VRManager::Instance->GetHiddenAreaMesh(eye);
  if (mesh != nullptr && mesh->IsDrawable()) {
    Graphics::Instance->PushRasterizerState(
        hiddenAreaMeshMaterial->rasterizerOverride);
    hiddenAreaMeshMaterial->Use();
    mesh->Draw();
    Graphics::Instance->PopRasterizerState();
  }
}
