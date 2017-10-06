//
//  Scene.cpp
//

#include <forward_list>
#include <Scene.h>
#include <Model.h>
#include <PointLight.h>
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

void dg::Scene::Render() {
  // Clear back buffer.
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Render params.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // Traverse scene tree and sort out different types of objects
  // into their own lists.
  std::forward_list<SceneObject*> remainingObjects;
  std::forward_list<Model*> models;
  std::forward_list<PointLight*> lights;
  std::forward_list<Camera*> cameras;
  remainingObjects.push_front(this);
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
      } else if (auto model = std::dynamic_pointer_cast<PointLight>(*child)) {
        lights.push_front(model.get());
      } else if (auto model = std::dynamic_pointer_cast<Camera>(*child)) {
        cameras.push_front(model.get());
      }
    }
  }

  // Render out of the first camera.
  if (cameras.empty()) return;
  Camera *camera = cameras.front();

  // Set up view.
  Transform view = camera->transform.Inverse();
  glm::mat4x4 projection = camera->GetProjectionMatrix(
      window->GetWidth() / window->GetHeight());

  // Render models.
  int i = 0;
  for (auto model = models.begin(); model != models.end(); model++) {
    (*model)->material->SetCameraPosition(view.Inverse().translation);
    // TODO: Support more than just the first light.
    if (!lights.empty()) {
      (*model)->material->SetLight(*lights.front());
    }
    (*model)->Draw(view.ToMat4(), projection);
    i++;
  }
}

