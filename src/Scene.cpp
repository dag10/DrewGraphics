//
//  Scene.cpp
//

#include <Scene.h>
#include <Model.h>
#include <Camera.h>
#include <vr/VRManager.h>

dg::Scene::Scene() : SceneObject() {}

void dg::Scene::Initialize() {
  if (enableVR) {
    // Disable glfw vsync, since IVRComposer::WaitGetPoses() will wait for
    // "running start" in 90hz anyway.
    glfwSwapInterval(0);

    // Initialize OpenVR.
    VRManager::Initialize();

    // Create framebuffers to render into.
    uint32_t vrWidth, vrHeight;
    vr::VRSystem()->GetRecommendedRenderTargetSize(&vrWidth, &vrHeight);
    // TODO: These framebuffers should be owned by dg::VRManager.
    leftFramebuffer = std::make_shared<FrameBuffer>(
      vrWidth, vrHeight, false, true);
    rightFramebuffer = std::make_shared<FrameBuffer>(
      vrWidth, vrHeight, false, true);

    // Create container for OpenVR tracked devices.
    auto vrContainer = std::make_shared<SceneObject>();
    AddChild(vrContainer);
    leftController = std::make_shared<SceneObject>();
    vrContainer->AddChild(leftController);
    rightController = std::make_shared<SceneObject>();
    vrContainer->AddChild(rightController);
  }

  // Create camera.
  mainCamera = std::make_shared<Camera>();
  mainCamera->transform.translation = glm::vec3(0, 1.5, 0);
  if (enableVR) {
    vrContainer->AddChild(mainCamera);
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

  if (enableVR) {
    // Update camera pose.
    const Transform *xfHmd = VRManager::Instance->GetHmdTransform();
    if (xfHmd != nullptr) {
      mainCamera->transform = *xfHmd;
    }

    // Update controller poses.
    const Transform *xfLeft = VRManager::Instance->GetLeftControllerTransform();
    leftController->enabled = (xfLeft != nullptr);
    if (xfLeft != nullptr) {
      leftController->transform = *xfLeft;
    }
    const Transform *xfRight = VRManager::Instance->GetRightControllerTransform();
    rightController->enabled = (xfRight != nullptr);
    if (xfRight != nullptr) {
      rightController->transform = *xfRight;
    }
  }
}

void dg::Scene::RenderFrame() {
  if (enableVR) {
    // Wait for "running start", and get latest poses.
    VRManager::Instance->WaitGetPoses();
    // TODO: Update transforms to render poses, then to game poses after render.

    // Render left and right eyes for VR.
    RenderFrame(vr::EVREye::Eye_Left);
    RenderFrame(vr::EVREye::Eye_Right);
    window->ResetViewport();
  }

  // Clear back buffer.
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Render params.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // TODO: If VR, just render a quad of the left eye instead.
  RenderScene(*mainCamera);
}

void dg::Scene::RenderFrame(vr::EVREye eye) {
  // Set up framebuffer and render the eye.
  std::shared_ptr<FrameBuffer> framebuffer =
    (eye == vr::EVREye::Eye_Left) ? leftFramebuffer : rightFramebuffer;
  framebuffer->Bind();
  glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // Render the scene.
  RenderScene(*mainCamera, true, eye);
  framebuffer->Unbind();

  // Submit frame to SteamVR.
  vr::Texture_t frameTexture;
  frameTexture.eColorSpace = vr::EColorSpace::ColorSpace_Auto;
  frameTexture.eType = vr::ETextureType::TextureType_OpenGL;
  frameTexture.handle =
    (void *)(long)framebuffer->GetColorTexture()->GetHandle();
  vr::VRCompositor()->Submit(
    eye, &frameTexture, nullptr, vr::EVRSubmitFlags::Submit_Default);
}

void dg::Scene::RenderScene(
  const Camera& camera, bool renderForVR, vr::EVREye eye) {
  // Render skybox.
  if (skybox != nullptr && skybox->enabled) {
    skybox->Draw(camera, *window);
  }

  // Traverse scene tree and sort out different types of objects
  // into their own lists.
  std::forward_list<SceneObject*> remainingObjects;
  std::forward_list<Model*> models;
  std::forward_list<Light*> lights;
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
        models.push_front(model.get());
      } else if (auto light = std::dynamic_pointer_cast<Light>(*child)) {
        lights.push_front(light.get());
      }
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
    projection = camera.GetProjectionMatrix(window->GetAspectRatio());
  }

  // Render models.
  Transform camera_SS = camera.SceneSpace();
  for (auto model = models.begin(); model != models.end(); model++) {
    PrepareModelForDraw(
        **model, camera_SS.translation, view, projection, lights);
    (*model)->Draw(view, projection);
  }
}

void dg::Scene::PrepareModelForDraw(
    const Model& model,
    glm::vec3 cameraPosition,
    glm::mat4x4 view,
    glm::mat4x4 projection,
    const std::forward_list<Light*>& lights) const {
  model.material->SetCameraPosition(cameraPosition);
  model.material->ClearLights();
  int lightIndex = 0;
  for (auto light = lights.begin(); light != lights.end(); light++) {
    if (lightIndex >= Light::MAX_LIGHTS) {
      break;
    }
    model.material->SetLight(lightIndex, **light);
    lightIndex++;
  }
}

