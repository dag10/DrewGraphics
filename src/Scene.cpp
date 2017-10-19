//
//  Scene.cpp
//

#include <Scene.h>
#include <Model.h>
#include <Camera.h>

dg::Scene::Scene() : SceneObject() {}

void dg::Scene::Update() {
  // Execute Update() for all behaviors.
  for (
      auto behavior = behaviors.begin();
      behavior != behaviors.end();
      behavior++) {
    (*behavior)->Update();
  }
}

void dg::Scene::RenderFrame() {
  // Clear back buffer.
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Render params.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  RenderScene(*mainCamera);
}

void dg::Scene::RenderScene(const Camera& camera) const {
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
  Transform camera_SS = camera.SceneSpace();
  glm::mat4x4 view = camera_SS.Inverse().ToMat4();
  glm::mat4x4 projection = camera.GetProjectionMatrix(
      window->GetWidth() / window->GetHeight());

  // Render models.
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

