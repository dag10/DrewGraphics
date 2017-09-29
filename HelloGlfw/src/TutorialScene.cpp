//
//  TutorialScene.h
//

#include "TutorialScene.h"
#include "EngineTime.h"

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
  // Configure global includes for all shader files.
  dg::Shader::SetVertexHead("assets/shaders/includes/vertex_head.glsl");
  dg::Shader::AddVertexSource("assets/shaders/includes/vertex_main.glsl");
  dg::Shader::SetFragmentHead("assets/shaders/includes/fragment_head.glsl");
  dg::Shader::AddFragmentSource("assets/shaders/includes/fragment_main.glsl");

  // Create shaders.
  solidColorShader = std::make_shared<Shader>(dg::Shader::FromFiles(
      "assets/shaders/solidcolor.v.glsl",
      "assets/shaders/solidcolor.f.glsl"));

  // Create wooden cube material.
  Material cubeMaterial;
  cubeMaterial.shader = solidColorShader;
  cubeMaterial.SetProperty("Lit", true);
  cubeMaterial.SetProperty("Albedo", glm::vec3(1.0f, 0.5f, 0.31f));
  cubeMaterial.SetProperty("LightColor", lightColor);
  cubeMaterial.SetProperty("AmbientStrength", 0.2f);
  cubeMaterial.SetProperty("SpecularStrength", 0.5f);
  cubeMaterial.SetProperty("DiffuseStrength", 1.0f);

  // Create cubes.
  int numCubes = sizeof(cubePositions) / sizeof(cubePositions[0]);
  for (int i = 0; i < numCubes; i++) {
    Model cube = Model(
          dg::Mesh::Cube,
          cubeMaterial,
          Transform::T(cubePositions[i]));
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
  
  // Set up cube material.
  solidColorShader->Use();
  solidColorShader->SetVec3("LightPosition", xfLight.translation);
  solidColorShader->SetVec3("CameraPosition", camera.transform.translation);

  // Render models.
  for (auto model = models.begin(); model != models.end(); model++) {
    model->Draw(view, projection);
  }

  // Render light source.
  solidColorShader->SetBool("Lit", false);
  solidColorShader->SetVec3("Albedo", lightColor);
  solidColorShader->SetMat4(
      "MATRIX_MVP", projection * view * xfLight * xfLightScale);
  Mesh::Cube->Use();
  Mesh::Cube->Draw();
  Mesh::Cube->FinishUsing();
}

