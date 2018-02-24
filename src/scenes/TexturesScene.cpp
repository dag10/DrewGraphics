//
//  scenes/TexturesScene.h
//

#include <scenes/TexturesScene.h>

#include <glm/glm.hpp>
#include <EngineTime.h>
#include <Canvas.h>
#include <forward_list>
#include <materials/StandardMaterial.h>
#include <behaviors/KeyboardCameraController.h>
#include <Lights.h>
#include <Window.h>
#include <Camera.h>
#include <Shader.h>
#include <Texture.h>
#include <Mesh.h>
#include <Model.h>
#include <Skybox.h>
#include <FrameBuffer.h>

std::unique_ptr<dg::TexturesScene> dg::TexturesScene::Make() {
  return std::unique_ptr<dg::TexturesScene>(new dg::TexturesScene());
}

dg::TexturesScene::TexturesScene() : Scene() {}

void dg::TexturesScene::Initialize() {
  Scene::Initialize();

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
  skybox = std::make_shared<Skybox>(skyboxTexture);

  // Create ceiling light source.
  auto ceilingLight = std::make_shared<PointLight>(
      glm::vec3(1.0f, 0.93f, 0.86f),
      0.732f, 0.399f, 0.968f);
  ceilingLight->transform.translation = glm::vec3(0.8f, 1.2f, -0.2f);
  AddChild(ceilingLight);

  // Create light sphere material.
  StandardMaterial lightMaterial = StandardMaterial::WithColor(
      ceilingLight->GetSpecular());
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
  framebuffer = std::make_shared<FrameBuffer>(1024, 1024, true, true);

  // Create material for displaying the framebuffer color buffer.
  StandardMaterial framebufferColorMaterial =
    StandardMaterial::WithTexture(framebuffer->GetColorTexture());
  framebufferColorMaterial.SetLit(false);

  // Create material for displaying the framebuffer depth buffer.
  StandardMaterial framebufferDepthMaterial =
    StandardMaterial::WithTexture(framebuffer->GetDepthTexture());
  framebufferDepthMaterial.SetLit(false);

  // Create solid color material for showing on framebuffer quads in
  // scened rendered to framebuffer.
  StandardMaterial dummyFramebufferMaterial = StandardMaterial::WithColor(
    glm::vec3(0.1, 0.15, 0.4));

  // Container for quads that show framebuffer, so they can be disabled
  // when rendering the virtual scene.
  renderQuads = std::make_shared<SceneObject>();
  renderQuads->transform = Transform::TRS(
    glm::vec3(0, 1.25f, -0.4),
    glm::quat(glm::radians(glm::vec3(-20, 0, 0))),
    glm::vec3(0.5f));
  AddChild(renderQuads);
  dummyRenderQuads = std::make_shared<SceneObject>();
  dummyRenderQuads->transform = renderQuads->transform;
  AddChild(dummyRenderQuads);

  // Create quad to show framebuffer color and depth buffers.
  float quadSize = 1.2f;
  float quadSep = 0.1f;
  renderQuads->AddChild(std::make_shared<Model>(
    dg::Mesh::Quad,
    std::make_shared<StandardMaterial>(framebufferColorMaterial),
    Transform::TS(
      glm::vec3(-quadSize * 0.5f - quadSep * 0.5f, 0, 0),
      quadSize * glm::vec3(1, 1 / window->GetAspectRatio(), 1))), false);
  renderQuads->AddChild(std::make_shared<Model>(
    dg::Mesh::Quad,
    std::make_shared<StandardMaterial>(framebufferDepthMaterial),
    Transform::TS(
      glm::vec3(quadSize * 0.5f + quadSep * 0.5f, 0, 0),
      quadSize * glm::vec3(1, 1 / window->GetAspectRatio(), 1))), false);
  dummyRenderQuads->AddChild(std::make_shared<Model>(
    dg::Mesh::Quad,
    std::make_shared<StandardMaterial>(dummyFramebufferMaterial),
    Transform::TS(
      glm::vec3(-quadSize * 0.5f - quadSep * 0.5f, 0, 0),
      quadSize * glm::vec3(1, 1 / window->GetAspectRatio(), 1))), false);
  dummyRenderQuads->AddChild(std::make_shared<Model>(
    dg::Mesh::Quad,
    std::make_shared<StandardMaterial>(dummyFramebufferMaterial),
    Transform::TS(
      glm::vec3(quadSize * 0.5f + quadSep * 0.5f, 0, 0),
      quadSize * glm::vec3(1, 1 / window->GetAspectRatio(), 1))), false);

  // Create custom texture using a Canvas.
  Canvas canvas(1, 6);
  canvas.SetPixel(0, 5, 75, 0, 130);
  canvas.SetPixel(0, 4, 0, 0, 255);
  canvas.SetPixel(0, 3, 0, 255, 0);
  canvas.SetPixel(0, 2, 255, 255, 0);
  canvas.SetPixel(0, 1, 255, 127, 0);
  canvas.SetPixel(0, 0, 255, 0, 0);
  canvas.Submit();

  // Create a quad to show the custom canvas.
  renderQuads->AddChild(std::make_shared<Model>(
    dg::Mesh::Quad,
    std::make_shared<StandardMaterial>(
      StandardMaterial::WithTexture(canvas.GetTexture())),
    Transform::TS(
      glm::vec3(quadSize * 1.5f + (2 * quadSep) * 0.5f, 0, 0),
      quadSize * glm::vec3(1, 1 / window->GetAspectRatio(), 1))), false);

  // Create virtual camera.
  virtualCamera = std::make_shared<Camera>();
  virtualCamera->transform.translation = glm::vec3(0, 1, 2);
  virtualCamera->LookAtPoint(glm::vec3(cube->transform.translation));
  virtualCamera->farClip = 7;
  virtualCamera->nearClip = 1;
  AddChild(virtualCamera);

  // Configure camera.
  mainCamera->transform.translation = glm::vec3(-1.25f, 2, 1.1f);
  mainCamera->LookAtPoint(
      (cube->transform.translation +
       ceilingLight->transform.translation) / 2.f);

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(
      mainCamera,
      std::make_shared<KeyboardCameraController>(window));

  // Allow the virtual camera to also be controller.
  Behavior::Attach(
      virtualCamera,
      std::make_shared<KeyboardCameraController>(window));
  virtualCamera->GetBehavior<KeyboardCameraController>()->enabled = false;
}

void dg::TexturesScene::Update() {
  Scene::Update();

  if (window->IsKeyJustPressed(Key::SPACE)) {
    flyingMainCamera = !flyingMainCamera;
    mainCamera->GetBehavior<KeyboardCameraController>()->enabled =
      flyingMainCamera;
    virtualCamera->GetBehavior<KeyboardCameraController>()->enabled =
      !flyingMainCamera;
  }

  if (flyingMainCamera) {
    virtualCamera->transform = Transform::R(glm::quat(glm::radians(
            glm::vec3(0, Time::Delta * 10, 0)))) * virtualCamera->transform;
  }
}

void dg::TexturesScene::RenderFrame() {
  // Render scene for framebuffer.
  framebuffer->Bind();
  glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());
  glClearColor(0, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  renderQuads->enabled = false;
  dummyRenderQuads->enabled = true;
  DrawScene(*virtualCamera);
  dummyRenderQuads->enabled = false;
  renderQuads->enabled = true;
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
  DrawScene(*mainCamera);
}

