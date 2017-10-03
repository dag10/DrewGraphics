//
//  TutorialScene.h
//

#include "TutorialScene.h"
#include "EngineTime.h"
#include "StandardMaterial.h"

#include <glm/glm.hpp>

static const glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
};

static const dg::Transform xfLightScale = dg::Transform::S(glm::vec3(0.1f));
static const glm::vec3 lightColor = glm::vec3(1, 1, 1);

std::unique_ptr<dg::TutorialScene> dg::TutorialScene::Make() {
  return std::unique_ptr<dg::TutorialScene>(new dg::TutorialScene());
}

void dg::TutorialScene::Initialize() {
  // Create wooden cube material.
  StandardMaterial cubeMaterial = StandardMaterial::WithColor(
      glm::vec3(1.0, 0.5f, 0.31f));
  cubeMaterial.SetAmbient(0.2f);
  cubeMaterial.SetDiffuse(1.0f);
  cubeMaterial.SetSpecular(0.5f);
  cubeMaterial.SetLightColor(lightColor);

  // Create cubes.
  int numCubes = sizeof(cubePositions) / sizeof(cubePositions[0]);
  Model cubeModel = Model(
      dg::Mesh::Cube,
      std::make_shared<StandardMaterial>(cubeMaterial),
      Transform());
  for (int i = 0; i < numCubes; i++) {
    Model cube = Model(cubeModel);
    cube.transform.translation = cubePositions[i];
    models.push_back(std::move(cube));
  }

  // Set initial camera position.
  camera.transform = Transform::T(glm::vec3(2, 0.0f, 4));
  camera.LookAtPoint(glm::vec3(0));
}

void dg::TutorialScene::Update() {
  // Calculate new rotation for camera, based on Left and Right keys.
  const float rotationSpeed = 180; // degrees per second
  Transform xfRotDelta = dg::Transform::R(glm::quat(glm::radians(
          glm::vec3(0, rotationSpeed * (float)Time::Delta, 0))));
  if (window->IsKeyPressed(GLFW_KEY_LEFT)) {
    camera.transform = xfRotDelta.Inverse() * camera.transform;
  }
  if (window->IsKeyPressed(GLFW_KEY_RIGHT)) {
    camera.transform = xfRotDelta * camera.transform;
  }

  // Calculate rotation for the first box based on the < and > keys.
  if (window->IsKeyPressed(GLFW_KEY_COMMA)) {
    models[0].transform = models[0].transform * xfRotDelta.Inverse();
  }
  if (window->IsKeyPressed(GLFW_KEY_PERIOD)) {
    models[0].transform = models[0].transform * xfRotDelta;
  }
  
  // Update light position.
  xfLight = Transform::T(glm::vec3(1.5f, 0.8f, 0.8f));
}

void dg::TutorialScene::Render() {
  // Set up view.
  glm::mat4x4 view = camera.GetViewMatrix();
  glm::mat4x4 projection = camera.GetProjectionMatrix(
      window->GetWidth() / window->GetHeight());

  // Clear back buffer.
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Render params.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // Render models.
  for (auto model = models.begin(); model != models.end(); model++) {
    model->material->SetLightPosition(xfLight.translation);
    model->material->SetCameraPosition(camera.transform.translation);
    model->Draw(view, projection);
  }

  // Render light source.
  // TODO: Create lightModel member.
  //solidColorShader->SetBool("Lit", false);
  //solidColorShader->SetVec3("Albedo", lightColor);
  //solidColorShader->SetMat4(
      //"MATRIX_MVP", projection * view * xfLight * xfLightScale);
  //Mesh::Cube->Use();
  //Mesh::Cube->Draw();
  //Mesh::Cube->FinishUsing();
}

