//
//  scenes/PortalScene.h
//

#include <scenes/PortalScene.h>

#include <memory>
#include <glm/glm.hpp>
#include <EngineTime.h>
#include <Texture.h>
#include <Mesh.h>
#include <Transform.h>
#include <forward_list>
#include <iostream>
#include <behaviors/KeyboardCameraController.h>
#include <behaviors/KeyboardLightController.h>
#include <lights/DirectionalLight.h>
#include <lights/PointLight.h>

static const glm::vec3 cubePositions[] = {
  glm::vec3(  0.0f,  0.25f,  0.0f ),
  glm::vec3( -1.0f,  0.25f,  0.0f ),
  glm::vec3(  1.0f,  0.25f,  0.0f ),
};

static dg::Transform portalTransforms[] = {
  dg::Transform::TR(
      glm::vec3(0, 0.6f, -1.5f + 0.001f),
      glm::quat(glm::radians(glm::vec3(0, 0, 0)))),
  dg::Transform::TR(
      glm::vec3(-1.0f, 0.6f, 0.8f),
      glm::quat(glm::radians(glm::vec3(0, 135, 0)))),
};

static const glm::vec3 backgroundColor = glm::vec3(0, 0, 0);

static const dg::Transform portalQuadScale = \
    dg::Transform::S(glm::vec3(1, 1.2f, 1));
static const dg::Transform portalOpeningScale = \
    dg::Transform::TS(
        glm::vec3(0, 0, 0.0003f), // Prevent z-fighting between back and stencil.
        glm::vec3(
          portalQuadScale.scale.x - (0.02f * 2.f),
          portalQuadScale.scale.y - (0.02f * 2.f),
          1));

std::unique_ptr<dg::PortalScene> dg::PortalScene::Make() {
  return std::unique_ptr<dg::PortalScene>(new dg::PortalScene());
}

dg::PortalScene::PortalScene() : Scene() {}

void dg::PortalScene::Initialize() {
  Scene::Initialize();

  // Lock window cursor to center.
  window->LockCursor();

  // Create shaders.
  depthResetShader = std::make_shared<Shader>(dg::Shader::FromFiles(
      "assets/shaders/depthreset.v.glsl",
      "assets/shaders/depthreset.f.glsl"));

  // Create textures.
  std::shared_ptr<Texture> crateTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/container2.png"));
  std::shared_ptr<Texture> crateSpecularTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/container2_specular.png"));
  std::shared_ptr<Texture> brickTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/brickwall.jpg"));
  std::shared_ptr<Texture> brickNormalTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/brickwall_normal.jpg"));
  std::shared_ptr<Texture> hardwoodTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/hardwood.jpg"));
  std::shared_ptr<Texture> rustyPlateTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/rustyplate.jpg"));
  std::shared_ptr<Texture> skyboxTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/skybox_daylight.png"));

  // Create skybox.
  skybox = std::unique_ptr<Skybox>(new Skybox(skyboxTexture));
  skybox->material.SetInvPortal(glm::mat4x4(0));

  // Create sky light.
  skyLight = std::make_shared<DirectionalLight>(
      glm::normalize(glm::vec3(-0.3f, -1, -0.2f)),
      glm::vec3(1.0f, 0.93f, 0.86f),
      0.34f, 1.45f, 0.07f);
  Behavior::Attach(skyLight, std::make_shared<KeyboardLightController>(window));
  AddChild(skyLight);

  // Create light cube material.
  StandardMaterial lightMaterial;
  lightMaterial.SetLit(false);

  // Create light cube.
  lightModel = std::make_shared<Model>(
      dg::Mesh::Cube,
      std::make_shared<StandardMaterial>(lightMaterial),
      Transform::TS(glm::vec3(1, 1.7f, 0), glm::vec3(0.05f)));
  AddChild(lightModel);

  // Create indoor and outdoor ceiling light source.
  glm::vec3 ceilingLightColor = glm::vec3(1.0f, 0.93f, 0.86f);
  spotLight = std::make_shared<SpotLight>(
      glm::vec3(0, -1, 0), glm::radians(35.f),
      ceilingLightColor, 0.286f, 1.344f, 2.21f);
  spotLight->feather = glm::radians(3.f);
  indoorCeilingLight = std::make_shared<PointLight>(
      ceilingLightColor, 0.927f, 0.903f, 1.063f);
  outdoorCeilingLight = std::make_shared<PointLight>(
      ceilingLightColor, 0.134f, 0.518f, 0.803f);
  lightModel->AddChild(spotLight, false);
  lightModel->AddChild(indoorCeilingLight, false);
  lightModel->AddChild(outdoorCeilingLight, false);
  Behavior::Attach(
    spotLight, std::make_shared<KeyboardLightController>(window));
  Behavior::Attach(
    indoorCeilingLight, std::make_shared<KeyboardLightController>(window));
  Behavior::Attach(
    outdoorCeilingLight, std::make_shared<KeyboardLightController>(window));

  // Create wooden cube material.
  StandardMaterial cubeMaterial = StandardMaterial::WithTexture(crateTexture);
  cubeMaterial.SetSpecular(crateSpecularTexture);
  cubeMaterial.SetShininess(64);

  // Create wooden cubes.
  auto cubes = std::make_shared<SceneObject>();
  AddChild(cubes);
  int numCubes = sizeof(cubePositions) / sizeof(cubePositions[0]);
  Model baseCubeModel(
      dg::Mesh::Cube,
      std::make_shared<StandardMaterial>(cubeMaterial),
      Transform::S(glm::vec3(0.5f)));
  for (int i = 0; i < numCubes; i++) {
    auto cube = std::make_shared<Model>(baseCubeModel);
    cube->transform.translation = cubePositions[i];
    cubes->AddChild(cube);
  }

  // Create wall material.
  StandardMaterial wallMaterial = StandardMaterial::WithTexture(brickTexture);
  wallMaterial.SetNormalMap(brickNormalTexture);
  wallMaterial.SetSpecular(0.2f);
  wallMaterial.SetShininess(64);

  // Create back wall.
  auto backWall = std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(wallMaterial),
        Transform::TRS(
          glm::vec3(1, 1, -1.5),
          glm::quat(glm::radians(glm::vec3(0))),
          glm::vec3(5, 2, 1)
          ));
  std::static_pointer_cast<StandardMaterial>(backWall->material)->SetUVScale(
      glm::vec2(5, 2));
  AddChild(backWall);

  // Create front wall, which is a copy of the back wall.
  auto frontWall = std::make_shared<Model>(*backWall);
  frontWall->transform = frontWall->transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(0, 180, 0))));
  frontWall->transform.translation.z *= -1;
  AddChild(frontWall);

  // Create left wall.
  auto leftWall = std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(wallMaterial),
        Transform::TRS(
          glm::vec3(-1.5f, 1, 0),
          glm::quat(glm::radians(glm::vec3(0, 90, 0))),
          glm::vec3(3, 2, 1)
          ));
  std::static_pointer_cast<StandardMaterial>(leftWall->material)->SetUVScale(
      glm::vec2(3, 2));
  AddChild(leftWall);

  // Create right wall, which is a copy of the left wall.
  auto rightWall = std::make_shared<Model>(*leftWall);
  rightWall->transform = rightWall->transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(0, 180, 0))));
  rightWall->transform.translation.x = 3.5f;
  AddChild(rightWall);

  // Create floor material.
  StandardMaterial floorMaterial = StandardMaterial::WithTexture(
      std::make_shared<Texture>(Texture::FromPath(
          "assets/textures/Flooring_Stone_001/Flooring_Stone_001_COLOR.png")));
  floorMaterial.SetNormalMap(
      std::make_shared<Texture>(Texture::FromPath(
          "assets/textures/Flooring_Stone_001/Flooring_Stone_001_NRM.png")));
  floorMaterial.SetSpecular(
      std::make_shared<Texture>(Texture::FromPath(
          "assets/textures/Flooring_Stone_001/Flooring_Stone_001_SPEC.png")));
  floorMaterial.SetShininess(9);
  floorMaterial.SetUVScale(glm::vec2(5, 3) * 2.f);
  floorMaterial.SetLit(true);

  // Create floor.
  auto floor = std::make_shared<Model>(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(floorMaterial),
        Transform::TRS(
          glm::vec3(1, 0, 0),
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(5, 3, 1)
          ));
  AddChild(floor);

  // Create ceiling material.
  StandardMaterial ceilingMaterial = StandardMaterial::WithTexture(
      hardwoodTexture);
  ceilingMaterial.SetUVScale(glm::vec2(5, 3));
  ceilingMaterial.SetLit(true);
  ceilingMaterial.SetSpecular(0.1f);
  ceilingMaterial.SetShininess(32);

  // Create ceiling, which is a copy of the floor.
  ceiling= std::make_shared<Model>(*floor);
  ceiling->material = std::make_shared<StandardMaterial>(ceilingMaterial);
  ceiling->transform = ceiling->transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(180, 0, 0))));
  ceiling->transform.translation.y = 2;
  AddChild(ceiling);

  // Create portal back materials.
  StandardMaterial portalBackMaterial;
  portalBackMaterial.SetSpecular(0.0f);

  // Create red portal model.
  auto redPortalModel = std::make_shared<Model>(
      dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(portalBackMaterial),
      portalTransforms[0] * portalQuadScale);
  std::static_pointer_cast<StandardMaterial>(redPortalModel->material)->
      SetDiffuse(glm::vec3(1, 0, 0));
  AddChild(redPortalModel);

  // Create blue portal model.
  auto bluePortalModel = std::make_shared<Model>(
      dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(portalBackMaterial),
      portalTransforms[1] * portalQuadScale);
  std::static_pointer_cast<StandardMaterial>(bluePortalModel->material)->
      SetDiffuse(glm::vec3(0, 0, 1));
  AddChild(bluePortalModel);

  // Create portal stencil material.
  portalStencilMaterial.SetLit(false);
  portalStencilMaterial.SetDiffuse(backgroundColor);
  portalStencilMaterial.SetInvPortal(glm::mat4x4(0));

  // Configure camera.
  mainCamera->transform.translation = glm::vec3(2.2f, 0.85f, 1);
  mainCamera->LookAtPoint(glm::vec3(0, mainCamera->transform.translation.y, 0));
  mainCamera->nearClip = 0.01f;
  mainCamera->farClip = 10;

  // Create a flashlight attached to the camera.
  flashlight = std::make_shared<SpotLight>(
      glm::vec3(0, 0, -1), glm::radians(25.f),
      ceilingLightColor, 0.314f, 2.16f, 2.11f);
  flashlight->transform = Transform::T(glm::vec3(0.1f, -0.1f, 0));
  Behavior::Attach(
    flashlight, std::make_shared<KeyboardLightController>(window));
  mainCamera->AddChild(flashlight, false);

  // Create box that represents the camera's position.
  mainCamera->AddChild(std::make_shared<Model>(
      dg::Mesh::Cube,
      std::make_shared<Material>(
        StandardMaterial::WithColor(glm::vec3(0.8f, 1.0f, 0.8f))),
      Transform::S(glm::vec3(0.2f, 0.1f, 0.1f))), false);

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(
      mainCamera,
      std::make_shared<KeyboardCameraController>(window));

  // Initial lighting configuration is the indoor point light.
  lightingType = PointLighting;
  UpdateLightingConfiguration();

  // Ceiling light is initially not moving.
  animatingLight = false;
}

void dg::PortalScene::Update() {
  Transform xfCameraBefore = mainCamera->SceneSpace();

  Scene::Update();

  Transform xfCameraAfter = mainCamera->SceneSpace();
  Transform xfDelta = xfCameraBefore.Inverse() * xfCameraAfter;

  // Find a test point that we check for crossing of a portal.
  // This point is the center of the frustum's near clip plane.
  dg::Transform xfTestPoint = xfCameraBefore * dg::Transform::T(
      FORWARD * mainCamera->nearClip);

  // Determine the before and after camera transforms relative to each portal.
  dg::Transform xfRedBefore = portalTransforms[0].Inverse() * xfTestPoint;
  dg::Transform xfRedAfter = xfRedBefore * xfDelta;
  dg::Transform xfBlueBefore = portalTransforms[1].Inverse() * xfTestPoint;
  dg::Transform xfBlueAfter = xfBlueBefore * xfDelta;

  // Have we passed through the blue portal?
  if (xfBlueBefore.translation.z >= 0 && xfBlueAfter.translation.z < 0 &&
      std::abs(xfBlueBefore.translation.x) < portalOpeningScale.scale.x / 2 &&
      std::abs(xfBlueBefore.translation.y) < portalOpeningScale.scale.y / 2 ) {

    // The transform of the portal we're moving "to", but flipped by 180 degrees
    // since we're exiting out the "back" of the portal.
    dg::Transform xfFlippedPortal = portalTransforms[0] * dg::Transform::R(
        glm::quat(glm::radians(glm::vec3(0, 180, 0))));

    // Camera is passing through the blue portal, so move it to the
    // red portal offset by its delta to the blue portal.
    mainCamera->transform = xfFlippedPortal * portalTransforms[1].Inverse() *
                        xfCameraAfter;

  // Have we passed through the red portal?
  } else if (xfRedBefore.translation.z >= 0 && xfRedAfter.translation.z < 0 &&
      std::abs(xfRedBefore.translation.x) < portalOpeningScale.scale.x / 2 &&
      std::abs(xfRedBefore.translation.y) < portalOpeningScale.scale.y / 2 ) {
    // The transform of the portal we're moving "to", but flipped by 180 degrees
    // since we're exiting out the "back" of the portal.
    dg::Transform xfFlippedPortal = portalTransforms[1] * dg::Transform::R(
        glm::quat(glm::radians(glm::vec3(0, 180, 0))));

    // Camera is passing through the red portal, so move it to the
    // blue portal offset by its delta to the red portal.
    mainCamera->transform = xfFlippedPortal * portalTransforms[0].Inverse() *
                        xfCameraAfter;
  }

  // If F was tapped, switch to the flashlight configuration.
  if (window->IsKeyJustPressed(GLFW_KEY_F)) {
    lightingType = FlashlightLighting;
    UpdateLightingConfiguration();
  }

  // If T was tapped, switch to the spotlight configuration.
  if (window->IsKeyJustPressed(GLFW_KEY_T)) {
    lightingType = SpotLighting;
    UpdateLightingConfiguration();
  }

  // If I was tapped, switch to the indoor point light configuration.
  if (window->IsKeyJustPressed(GLFW_KEY_I)) {
    lightingType = PointLighting;
    UpdateLightingConfiguration();
  }

  // If O was tapped, switch to the outdoor configuration.
  if (window->IsKeyJustPressed(GLFW_KEY_O)) {
    lightingType = OutdoorLighting;
    UpdateLightingConfiguration();
  }

  // Toggle animating light with keyboard tap of L.
  if (window->IsKeyJustPressed(GLFW_KEY_L)) {
    animatingLight = !animatingLight;
  }

  // Animate light position.
  if (animatingLight) {
    lightModel->transform.translation.x = 1 + 1 * (float)sin(5 * Time::Elapsed);
  } else {
    lightModel->transform.translation.x = 1.5f;
  }

  // Update light cube model to be consistent with point light.
  std::shared_ptr<Light> activeLight = nullptr;
  switch (lightingType) {
    case OutdoorLighting:
      activeLight = outdoorCeilingLight;
      break;
    case PointLighting:
      activeLight = indoorCeilingLight;
      break;
    case SpotLighting:
      activeLight = spotLight;
      break;
    case FlashlightLighting:
      activeLight = flashlight;
      break;
  }
  std::static_pointer_cast<StandardMaterial>(lightModel->material)
    ->SetDiffuse(activeLight->specular);
}

void dg::PortalScene::PrepareModelForDraw(
    const Model& model,
    glm::vec3 cameraPosition,
    glm::mat4x4 view,
    glm::mat4x4 projection,
    const std::forward_list<Light*>& lights) const {
  Scene::PrepareModelForDraw(model, cameraPosition, view, projection, lights);
  model.material->SetInvPortal(invPortal);
}

void dg::PortalScene::UpdateLightingConfiguration() {
  skybox->enabled = (lightingType == OutdoorLighting);
  ceiling->enabled = (lightingType != OutdoorLighting);
  skyLight->enabled = (lightingType == OutdoorLighting);
  lightModel->enabled = (lightingType != FlashlightLighting);
  indoorCeilingLight->enabled = (lightingType == PointLighting);
  outdoorCeilingLight->enabled = (lightingType == OutdoorLighting);
  spotLight->enabled = (lightingType == SpotLighting);
  flashlight->enabled = (lightingType == FlashlightLighting);
}

void dg::PortalScene::RenderPortalStencil(dg::Transform xfPortal) {
  glm::mat4x4 view = mainCamera->GetViewMatrix();
  glm::mat4x4 projection = mainCamera->GetProjectionMatrix(
    window->GetAspectRatio());

  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glDepthFunc(GL_LEQUAL);
  glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
  glClear(GL_STENCIL_BUFFER_BIT);

  portalStencilMaterial.SetMatrixMVP(
      projection * view * xfPortal * portalOpeningScale);
  portalStencilMaterial.Use();

  Mesh::Quad->Use();
  Mesh::Quad->Draw();
  Mesh::Quad->FinishUsing();

  glDisable(GL_STENCIL_TEST);
}

void dg::PortalScene::ClearDepth() {
  glDepthFunc(GL_ALWAYS);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  depthResetShader->Use();
  Mesh::Quad->Use();
  Mesh::Quad->Draw();
  Mesh::Quad->FinishUsing();

  glDepthFunc(GL_LESS);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

dg::Camera dg::PortalScene::CameraForPortal(
    Transform inPortal, Transform outPortal) {
  // Copy scene camera.
  Camera camera(*mainCamera);

  // Flip out portal around.
  dg::Transform flippedOutPortal = outPortal * dg::Transform::R(
      glm::quat(glm::radians(glm::vec3(0, 180, 0))));

  // Calculate the this portal camera's transform by applying the delta of the
  // two portals to the main camera's transform.
  camera.transform = flippedOutPortal * inPortal.Inverse() * camera.transform;

  return camera;
}

void dg::PortalScene::RenderFrame() {
  // Clear back buffer.
  glClearColor(
      backgroundColor.x, backgroundColor.y, backgroundColor.z, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Render params.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // Render immediate scene.
  invPortal = glm::mat4x4(0);
  RenderScene(*mainCamera);

  // Render first (red) portal stencil.
  RenderPortalStencil(portalTransforms[0]);

  // Render scene through first (red) portal.
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  ClearDepth(); // Clear depth buffer only within stencil.
  invPortal = portalTransforms[1].Inverse().ToMat4();
  RenderScene(CameraForPortal(portalTransforms[0], portalTransforms[1]));
  glDisable(GL_STENCIL_TEST);

  // Render first (red) portal stencil.
  RenderPortalStencil(portalTransforms[1]);

  // Render scene through second (blue) portal.
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  ClearDepth(); // Clear depth buffer only within stencil.
  invPortal = portalTransforms[0].Inverse().ToMat4();
  RenderScene(CameraForPortal(portalTransforms[1], portalTransforms[0]));
  glDisable(GL_STENCIL_TEST);
}

void dg::PortalScene::RenderScene(
  const Camera& camera, bool renderForVR, vr::EVREye eye) {
  // Move the flash light to the camera we're rendering from.
  // TODO: Don't do this, it feels very computationally heavy.
  Transform xfFlashlightOriginal = flashlight->transform;
  flashlight->SetSceneSpace(
    camera.SceneSpace() * mainCamera->SceneSpace().Inverse() *
    flashlight->SceneSpace());

  Scene::RenderScene(camera, renderForVR, eye);

  // Attach the flashlight back to the original camera.
  flashlight->transform = xfFlashlightOriginal;
}

