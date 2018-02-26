//
//  Scene.cpp
//

#include "dg/Scene.h"
#include "dg/Camera.h"
#include "dg/Graphics.h"
#include "dg/Lights.h"
#include "dg/Model.h"
#include "dg/Skybox.h"
#include "dg/Window.h"

#pragma region Base Class

dg::BaseScene::BaseScene() : SceneObject() {}
dg::BaseScene::~BaseScene() {}

void dg::BaseScene::Initialize() {
  // Create camera.
  mainCamera = std::make_shared<Camera>();
  mainCamera->transform.translation = glm::vec3(0, 1.5, 0);
  AddChild(mainCamera);
}

void dg::BaseScene::Update() {
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

void dg::BaseScene::ClearBuffer() {
  Graphics::Instance->Clear(glm::vec3(0));
}

void dg::BaseScene::RenderFrame() {
  ClearBuffer();
  ConfigureBuffer();
  DrawScene(*mainCamera);
}

void dg::BaseScene::DrawScene(const Camera& camera) {
  // Render skybox.
  if (skybox != nullptr && skybox->enabled) {
    skybox->Draw(camera, *window);
  }

  ConfigureBuffer();

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
  view = camera.GetViewMatrix();
  projection = camera.GetProjectionMatrix(window->GetAspectRatio());

  // Prepare light data.
  Light::ShaderData lightArray[Light::MAX_LIGHTS];
  int lightIdx = 0;
  for (auto light = lights.begin(); light != lights.end(); light++) {
    if (lightIdx >= Light::MAX_LIGHTS) {
      break;
    }
    lightArray[lightIdx++] = (*light)->GetShaderData();
  }

  // Render models.
  Transform camera_SS = camera.SceneSpace();
  for (auto model = models.begin(); model != models.end(); model++) {
    PrepareModelForDraw(
        **model, camera_SS.translation, view, projection, lightArray);
    (*model)->Draw(view, projection);
  }
}

void dg::BaseScene::PrepareModelForDraw(
    const Model& model, glm::vec3 cameraPosition, glm::mat4x4 view,
    glm::mat4x4 projection,
    const Light::ShaderData (&lights)[Light::MAX_LIGHTS]) const {

  model.material->SendCameraPosition(cameraPosition);
  model.material->SendLights(lights);
}

bool dg::BaseScene::AutomaticWindowTitle() const {
  return true;
}

#pragma endregion
#pragma region OpenGL Scene
#pragma endregion
#pragma region DirectX Scene

void dg::DirectXScene::ConfigureBuffer() {
}

#pragma endregion
