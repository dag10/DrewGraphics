//
//  scenes/QuadScene.h
//

#include <scenes/QuadScene.h>

#include <glm/glm.hpp>
#include <materials/StandardMaterial.h>
#include <lights/PointLight.h>
#include <lights/DirectionalLight.h>

std::unique_ptr<dg::QuadScene> dg::QuadScene::Make() {
  return std::unique_ptr<dg::QuadScene>(new dg::QuadScene());
}

dg::QuadScene::QuadScene() : Scene() {}

void dg::QuadScene::Initialize() {
  // Create skybox, disabled by default.
  skybox = std::unique_ptr<Skybox>(new Skybox(
        std::make_shared<Texture>(Texture::FromPath(
            "assets/textures/skybox_daylight.png"))));
  skybox->enabled = false;

  // Create quad material.
  auto quadMaterial = StandardMaterial::WithTexture(std::make_shared<Texture>(
        Texture::FromPath("assets/textures/container2.png")));
  quadMaterial.SetSpecular(std::make_shared<Texture>(
        Texture::FromPath("assets/textures/container2_specular.png")));
  quadMaterial.SetShininess(64);

  // Create quad.
  quad = std::make_shared<Model>(
      dg::Mesh::Quad,
      std::make_shared<StandardMaterial>(quadMaterial),
      Transform());
  AddChild(quad);

  // Create point light just behind the camera, to the top-right.
  auto pointLight = std::make_shared<PointLight>(
      glm::vec3(1.0f, 0.9f, 0.8f), // Warmish light
      0.232f, 0.399f, 0.968f);
  pointLight->transform.translation = glm::vec3(1, 1, 1);
  AddChild(pointLight);

  // Create directional fill light.
  auto directionalLight = std::make_shared<DirectionalLight>(
      glm::normalize(glm::vec3(0.5f, -0.9f, -0.2)),
      glm::vec3(1), // White light
      0.132f, 0.4f, 0.568f);
  AddChild(directionalLight);

  // Create camera.
  mainCamera = std::make_shared<Camera>();
  mainCamera->transform.translation = glm::vec3(0, 0, 2);
  mainCamera->LookAt(*quad);
  AddChild(mainCamera);
}

void dg::QuadScene::Update() {
  Scene::Update();

  // Slightly rotate quad based on cursor position.
  glm::vec2 absoluteCursorPos =
    window->GetCursorPosition() - (window->GetSize() / 4.f);
  glm::vec2 relativeCursorPos = absoluteCursorPos / window->GetSize();
  relativeCursorPos.x *= window->GetWidth() / window->GetHeight();
  glm::vec3 quadLookTarget = glm::vec3(
      sin(-relativeCursorPos.x),
      sin(relativeCursorPos.y),
      -0.1f);
  quad->LookAtPoint(quadLookTarget);

  // Toggle the skybox with a tap of the S key.
  if (window->IsKeyJustPressed(GLFW_KEY_S)) {
    skybox->enabled = !skybox->enabled;
  }
}

