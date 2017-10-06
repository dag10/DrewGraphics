//
//  scenes/TutorialScene.h
//

#include <scenes/TutorialScene.h>

#include <glm/glm.hpp>
#include <EngineTime.h>
#include <forward_list>
#include <materials/StandardMaterial.h>
#include <behaviors/KeyboardCameraController.h>

std::unique_ptr<dg::TutorialScene> dg::TutorialScene::Make() {
  return std::unique_ptr<dg::TutorialScene>(new dg::TutorialScene());
}

dg::TutorialScene::TutorialScene() : Scene() {}

void dg::TutorialScene::Initialize() {
  // Lock window cursor to center.
  window->LockCursor();

  // Create textures.
  std::shared_ptr<Texture> crateTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/container2.png"));
  std::shared_ptr<Texture> crateSpecularTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/container2_specular.png"));
  std::shared_ptr<Texture> hardwoodTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/hardwood.jpg"));

  // Create ceiling light source.
  auto ceilingLight = std::make_shared<PointLight>(
      glm::vec3(1.0f, 0.93f, 0.86f),
      0.732f, 0.399f, 0.968f);
  ceilingLight->transform.translation = glm::vec3(0.8f, 1.2f, -0.2f);
  AddChild(ceilingLight);

  // Create light cube material.
  StandardMaterial lightMaterial = StandardMaterial::WithColor(
      ceilingLight->specular);
  lightMaterial.SetLit(false);

  // Create light cube.
  auto lightModel = std::make_shared<Model>(
      dg::Mesh::Cube,
      std::make_shared<StandardMaterial>(lightMaterial),
      Transform::S(glm::vec3(0.05f)));
  ceilingLight->AddChild(lightModel, false);

  // Create wooden cube material.
  StandardMaterial cubeMaterial = StandardMaterial::WithTexture(crateTexture);
  cubeMaterial.SetSpecular(crateSpecularTexture);
  cubeMaterial.SetShininess(64);

  // Create wooden cube.
  auto cube = std::make_shared<Model>(
      dg::Mesh::Cube,
      std::make_shared<StandardMaterial>(cubeMaterial),
      Transform::TS(glm::vec3(0, 0.25f, 0), glm::vec3(0.5f)));
  AddChild(cube);

  // Create floor material.
  const int floorSize = 100;
  StandardMaterial floorMaterial = StandardMaterial::WithTexture(
      hardwoodTexture);
  floorMaterial.SetUVScale(glm::vec2(floorSize));

  // Create floor plane.
  AddChild(std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(floorMaterial),
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(floorSize, floorSize, 1))));

  // Create camera.
  camera = std::make_shared<Camera>();
  camera->transform.translation = glm::vec3(-1.25f, 2, 1.1f);
  camera->LookAtPoint(
      (cube->transform.translation +
       ceilingLight->transform.translation) / 2.f);
  AddChild(camera);

  // Allow camera to be controller by the keyboard and mouse.
  behaviors.push_back(
      std::unique_ptr<Behavior>(new KeyboardCameraController(camera, window)));
}

void dg::TutorialScene::Update() {
  Scene::Update();
}

void dg::TutorialScene::Render() {
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

