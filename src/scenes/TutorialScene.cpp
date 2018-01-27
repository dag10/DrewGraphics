//
//  scenes/TutorialScene.h
//

#include <scenes/TutorialScene.h>

#include <glm/glm.hpp>
#include <EngineTime.h>
#include <forward_list>
#include <materials/StandardMaterial.h>
#include <behaviors/KeyboardCameraController.h>
#include <lights/PointLight.h>

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
  std::shared_ptr<Texture> skyboxTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/skybox_daylight.png"));

  // Create skybox.
  skybox = std::unique_ptr<Skybox>(new Skybox(skyboxTexture));

  // Create ceiling light source.
  auto ceilingLight = std::make_shared<PointLight>(
      glm::vec3(1.0f, 0.93f, 0.86f),
      0.732f, 0.399f, 0.968f);
  ceilingLight->transform.translation = glm::vec3(0.8f, 1.2f, -0.2f);
  AddChild(ceilingLight);

  // Create light sphere material.
  StandardMaterial lightMaterial = StandardMaterial::WithColor(
      ceilingLight->specular);
  lightMaterial.SetLit(false);

  // Create light sphere.
  auto lightModel = std::make_shared<Model>(
      dg::Mesh::Sphere,
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
  const int floorSize = 10;
  StandardMaterial floorMaterial = StandardMaterial::WithTexture(
      hardwoodTexture);
  floorMaterial.SetUVScale(glm::vec2((float)floorSize));

  // Create floor plane.
  AddChild(std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(floorMaterial),
        Transform::RS(
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(floorSize, floorSize, 1))));

  // Create frame buffer.
  framebuffer = std::make_shared<FrameBuffer>(2048, 2048);

  // Create material for displaying the framebuffer.
  StandardMaterial framebufferMaterial =
    StandardMaterial::WithTexture(framebuffer->GetColorTexture());
  framebufferMaterial.SetLit(false);

  // Create quad to show framebuffer.
  renderQuad = std::make_shared<Model>(
    dg::Mesh::Quad,
    std::make_shared<StandardMaterial>(framebufferMaterial),
    Transform::TS(glm::vec3(0, 1.25f, -1), glm::vec3(0.5f)));
  AddChild(renderQuad);

  // Create virtual camera.
  virtualCamera = std::make_shared<Camera>();
  virtualCamera->transform.translation = glm::vec3(0, 1, 2);
  virtualCamera->LookAtPoint(glm::vec3(cube->transform.translation));
  AddChild(virtualCamera);

  // Create camera.
  mainCamera = std::make_shared<Camera>();
  mainCamera->transform.translation = glm::vec3(-1.25f, 2, 1.1f);
  mainCamera->LookAtPoint(
      (cube->transform.translation +
       ceilingLight->transform.translation) / 2.f);
  AddChild(mainCamera);

  // Allow camera to be controller by the keyboard and mouse.
  behaviors.push_back(std::unique_ptr<Behavior>(
        new KeyboardCameraController(mainCamera, window)));
}

void dg::TutorialScene::Update() {
  Scene::Update();

  virtualCamera->transform = Transform::R(glm::quat(glm::radians(
    glm::vec3(0, Time::Delta * 10, 0)))) * virtualCamera->transform;
}

void dg::TutorialScene::RenderFrame() {
  // Render scene for framebuffer.
  framebuffer->Bind();
  glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());
  glClearColor(0, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  renderQuad->enabled = false;
  RenderScene(*virtualCamera);
  renderQuad->enabled = true;
  framebuffer->Unbind();
  glViewport(
    0, 0, (GLsizei)window->GetWidth() * 2, (GLsizei)window->GetHeight() * 2);

  // Clear back buffer.
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Render params.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // Render scene for real.
  RenderScene(*mainCamera);
}
