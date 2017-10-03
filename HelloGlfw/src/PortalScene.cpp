//
//  PortalScene.h
//

#include "PortalScene.h"

#include <glm/glm.hpp>
#include "EngineTime.h"
#include "Texture.h"
#include "Mesh.h"
#include "Transform.h"

static const glm::vec3 cubePositions[] = {
  glm::vec3(  0.0f,  0.25f,  0.0f ), 
  glm::vec3( -1.0f,  0.25f,  0.0f ), 
  glm::vec3(  1.0f,  0.25f,  0.0f ), 
};

static const glm::vec3 lightColor = glm::vec3(0.7f, 0.65f, 0.6f);

static dg::Transform portalTransforms[] = {
  dg::Transform::TR(
      glm::vec3(0, 0.6f, -1.5f + 0.001f),
      glm::quat(glm::radians(glm::vec3(0, 0, 0)))),
  dg::Transform::TR(
      glm::vec3(-1.5f + 0.001f, 0.6f, 0),
      glm::quat(glm::radians(glm::vec3(0, 90, 0)))),
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

void dg::PortalScene::Initialize() {
  // Lock window cursor to center.
  window->LockCursor();

  // Create shaders.
  depthResetShader = std::make_shared<Shader>(dg::Shader::FromFiles(
      "assets/shaders/depthreset.v.glsl",
      "assets/shaders/depthreset.f.glsl"));

  // Create textures.
  std::shared_ptr<Texture> crateTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/container.jpg"));
  std::shared_ptr<Texture> brickTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/brick.png"));
  std::shared_ptr<Texture> hardwoodTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/hardwood.jpg"));
  std::shared_ptr<Texture> rustyPlateTexture = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/rustyplate.jpg"));

  // Set light position.
  xfLight = Transform::T(glm::vec3(2, 1.7f, 0));

  // Create light cube.
  StandardMaterial lightMaterial = StandardMaterial::WithColor(lightColor);
  lightMaterial.SetLit(false);
  Model lightCube = Model(
      dg::Mesh::Cube,
      std::make_shared<StandardMaterial>(lightMaterial),
      xfLight * Transform::S(glm::vec3(0.05f)));
  models.push_back(std::move(lightCube));

  // Create wooden cube material.
  StandardMaterial cubeMaterial = StandardMaterial::WithTexture(crateTexture);
  cubeMaterial.SetAmbient(0.8f);
  cubeMaterial.SetDiffuse(1.0f);
  cubeMaterial.SetSpecular(0.2f);

  // Create wooden cubes.
  int numCubes = sizeof(cubePositions) / sizeof(cubePositions[0]);
  Model cubeModel = Model(
      dg::Mesh::Cube,
      std::make_shared<StandardMaterial>(cubeMaterial),
      Transform::S(glm::vec3(0.5f)));
  for (int i = 0; i < numCubes; i++) {
    Model cube = Model(cubeModel);
    cube.transform.translation = cubePositions[i];
    models.push_back(std::move(cube));
  }

  // Create wall material.
  StandardMaterial wallMaterial = StandardMaterial::WithTexture(brickTexture);
  wallMaterial.SetAmbient(0.4f);
  wallMaterial.SetDiffuse(0.8f);
  wallMaterial.SetSpecular(0.4f);

  // Create back wall.
  Model backWall = Model(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(wallMaterial),
        Transform::TRS(
          glm::vec3(1, 1, -1.5),
          glm::quat(glm::radians(glm::vec3(0))),
          glm::vec3(5, 2, 1)
          ));
  std::static_pointer_cast<StandardMaterial>(backWall.material)->SetUVScale(
      glm::vec2(5, 2));
  models.push_back(Model(backWall));

  // Create front wall.
  Model frontWall = backWall;
  frontWall.transform = frontWall.transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(0, 180, 0))));
  frontWall.transform.translation.z *= -1;
  models.push_back(Model(frontWall));

  // Create left wall.
  Model leftWall = Model(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(wallMaterial),
        Transform::TRS(
          glm::vec3(-1.5f, 1, 0),
          glm::quat(glm::radians(glm::vec3(0, 90, 0))),
          glm::vec3(3, 2, 1)
          ));
  std::static_pointer_cast<StandardMaterial>(leftWall.material)->SetUVScale(
      glm::vec2(3, 2));
  models.push_back(Model(leftWall));

  // Create right wall.
  Model rightWall = leftWall;
  rightWall.transform = rightWall.transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(0, 180, 0))));
  rightWall.transform.translation.x = 3.5f;
  models.push_back(Model(rightWall));

  // Create floor material.
  StandardMaterial floorMaterial = StandardMaterial::WithTexture(
      rustyPlateTexture);
  floorMaterial.SetUVScale(glm::vec2(5, 3) * 2.f);
  floorMaterial.SetLit(true);
  floorMaterial.SetAmbient(0.4f);
  floorMaterial.SetDiffuse(0.8f);
  floorMaterial.SetSpecular(0.4f);

  // Create floor.
  Model floor = Model(
        dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(floorMaterial),
        Transform::TRS(
          glm::vec3(1, 0, 0),
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(5, 3, 1)
          ));
  models.push_back(Model(floor));

  // Create ceiling material.
  StandardMaterial ceilingMaterial = floorMaterial;
  ceilingMaterial.SetAlbedo(hardwoodTexture);
  ceilingMaterial.SetSpecular(0.1f);
  ceilingMaterial.SetUVScale(glm::vec2(5, 3));

  // Create ceiling.
  Model ceiling = floor;
  ceiling.material = std::make_shared<StandardMaterial>(ceilingMaterial);
  ceiling.transform = ceiling.transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(180, 0, 0))));
  ceiling.transform.translation.y = 2;
  models.push_back(Model(ceiling));

  // Create portal back materials.
  StandardMaterial portalBackMaterial;
  portalBackMaterial.SetAmbient(0.4f);
  portalBackMaterial.SetDiffuse(0.8f);
  portalBackMaterial.SetSpecular(0.0f);

  // Create portal models.
  Model redPortalModel = Model(
      dg::Mesh::Quad,
        std::make_shared<StandardMaterial>(portalBackMaterial),
      portalTransforms[0] * portalQuadScale);
  std::static_pointer_cast<StandardMaterial>(redPortalModel.material)->
      SetAlbedo(glm::vec3(1, 0, 0));
  Model bluePortalModel = Model(redPortalModel);
  std::static_pointer_cast<StandardMaterial>(bluePortalModel.material)->
      SetAlbedo(glm::vec3(1, 0, 0));
  bluePortalModel.transform = portalTransforms[1] * portalQuadScale;
  models.push_back(std::move(redPortalModel));
  models.push_back(std::move(bluePortalModel));

  // Create portal stencil material.
  portalStencilMaterial.SetLit(false);
  portalStencilMaterial.SetAlbedo(backgroundColor);
  portalStencilMaterial.SetInvPortal(glm::mat4x4(0));

  // Set initial camera position.
  camera.transform.translation = glm::vec3(2.2f, 0.85f, 1);
  camera.LookAtPoint(glm::vec3(0, camera.transform.translation.y, 0));

  // Temporarily shorten near clip plane to mask portal clipping
  // while moving through a portal.
  camera.nearClip = 0.01f;
  camera.farClip = 10;
}

void dg::PortalScene::Update() {
  const float speed = 1.8f; // units per second
  const float rotationSpeed = 90; // degrees per second
  const float cursorRotationSpeed = 0.3f; // degrees per cursor pixels moved

  // Calculate new rotation for camera based on mouse.
  if (window->IsCursorLocked()) {
    glm::vec2 cursorDelta = window->GetCursorDelta();
    glm::quat pitch = glm::quat(glm::radians(glm::vec3(
            -cursorDelta.y * cursorRotationSpeed,
            0,
            0)));
    glm::quat yaw = glm::quat(glm::radians(glm::vec3(
            0,
            -cursorDelta.x * cursorRotationSpeed,
            0)));
    glm::quat rotation = yaw * camera.transform.rotation *
      pitch * glm::inverse(camera.transform.rotation);
    camera.LookAtDirection(rotation * camera.transform.Forward());
  }

  // Calculate new movement relative to camera, based on WASD keys.
  glm::vec3 movementDir(0);
  if (window->IsKeyPressed(GLFW_KEY_W) || window->IsKeyPressed(GLFW_KEY_UP)) {
    movementDir += FORWARD;
  }
  if (window->IsKeyPressed(GLFW_KEY_S) || window->IsKeyPressed(GLFW_KEY_DOWN)) {
    movementDir += -FORWARD;
  }
  if (window->IsKeyPressed(GLFW_KEY_A)) {
    movementDir += -RIGHT;
  }
  if (window->IsKeyPressed(GLFW_KEY_D)) {
    movementDir += RIGHT;
  }
  float speedMultiplier =
    (window->IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) ? 2.f : 1.f;
  dg::Transform xfDelta = dg::Transform::T(
      movementDir * speed * speedMultiplier * (float)Time::Delta);

  // Find a test point that we check for crossing of a portal.
  // This point is the center of the frustum's near clip plane.
  dg::Transform xfTestPoint = camera.transform * dg::Transform::T(
      FORWARD * camera.nearClip);

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
    camera.transform = xfFlippedPortal * portalTransforms[1].Inverse() *
                       camera.transform;

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
    camera.transform = xfFlippedPortal * portalTransforms[0].Inverse() *
                       camera.transform;
  }

  // Apply delta to camera.
  camera.transform = camera.transform * xfDelta;
}

void dg::PortalScene::RenderScene(
    bool throughPortal, dg::Transform inPortal, dg::Transform outPortal) {

  // Set up view.
  Transform view = camera.transform.Inverse();
  glm::mat4x4 projection = camera.GetProjectionMatrix(
      window->GetWidth() / window->GetHeight());

  // If through a portal, transform the view matrix.
  if (throughPortal) {
    // Flip out portal around.
    dg::Transform flippedOutPortal = outPortal * dg::Transform::R(
        glm::quat(glm::radians(glm::vec3(0, 180, 0))));

    // Find delta between the two portals.
    dg::Transform xfDelta = inPortal * flippedOutPortal.Inverse();

    // Apply view transform to the portal delta instead of the origin.
    view = view * xfDelta;
  }

  // Render models.
  glm::mat4x4 invPortal = throughPortal ? outPortal.Inverse().ToMat4()
                                      : glm::mat4x4(0);
  int i = 0;
  for (auto model = models.begin(); model != models.end(); model++) {
    model->material->SetCameraPosition(view.Inverse().translation);
    model->material->SetInvPortal(invPortal);
    model->material->SetLightPosition(xfLight.translation);
    model->material->SetLightColor(lightColor);
    model->Draw(view.ToMat4(), projection);
    i++;
  }
}

void dg::PortalScene::RenderPortalStencil(dg::Transform xfPortal) {
  glm::mat4x4 view = camera.GetViewMatrix();
  glm::mat4x4 projection = camera.GetProjectionMatrix(
      window->GetWidth() / window->GetHeight());

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

void dg::PortalScene::Render() {
  // Clear back buffer.
  glClearColor(
      backgroundColor.x, backgroundColor.y, backgroundColor.z, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Render params.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  // Render immediate scene.
  RenderScene(false, dg::Transform(), dg::Transform());

  // Render first (red) portal stencil.
  RenderPortalStencil(portalTransforms[0]);

  // Render scene through first (red) portal.
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  ClearDepth(); // Clear depth buffer only within stencil.
  RenderScene(true, portalTransforms[0], portalTransforms[1]);
  glDisable(GL_STENCIL_TEST);

  // Render first (red) portal stencil.
  RenderPortalStencil(portalTransforms[1]);

  // Render scene through second (blue) portal.
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  ClearDepth(); // Clear depth buffer only within stencil.
  RenderScene(true, portalTransforms[1], portalTransforms[0]);
  glDisable(GL_STENCIL_TEST);
}

