//
//  PortalScene.h
//

#include "PortalScene.h"

#include <glm/glm.hpp>
#include "EngineTime.h"
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

  // Configure global includes for all shader files.
  dg::Shader::SetVertexHead("assets/shaders/includes/vertex_head.glsl");
  dg::Shader::AddVertexSource("assets/shaders/includes/vertex_main.glsl");
  dg::Shader::SetFragmentHead("assets/shaders/includes/fragment_head.glsl");
  dg::Shader::AddFragmentSource("assets/shaders/includes/fragment_main.glsl");

  // Create shaders.
  depthResetShader = std::make_shared<Shader>(dg::Shader::FromFiles(
      "assets/shaders/depthreset.v.glsl",
      "assets/shaders/depthreset.f.glsl"));
  simpleTextureShader = std::make_shared<Shader>(dg::Shader::FromFiles(
      "assets/shaders/simpletexture.v.glsl",
      "assets/shaders/simpletexture.f.glsl"));
  solidColorShader = std::make_shared<Shader>(dg::Shader::FromFiles(
      "assets/shaders/solidcolor.v.glsl",
      "assets/shaders/solidcolor.f.glsl"));

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
  Model lightCube = Model(
      dg::Mesh::Cube,
      solidColorShader,
      xfLight * Transform::S(glm::vec3(0.05f)));
  lightCube.lit = false;
  lightCube.albedo = lightColor;
  models.push_back(std::move(lightCube));

  // Create wooden cubes.
  int numCubes = sizeof(cubePositions) / sizeof(cubePositions[0]);
  for (int i = 0; i < numCubes; i++) {
    Model cube = Model(
          dg::Mesh::Cube,
          simpleTextureShader,
          crateTexture,
          glm::vec2(1),
          Transform::TS(cubePositions[i], glm::vec3(0.5f)));
    cube.lit = true;
    cube.albedo = glm::vec3(1.0f, 0.5f, 0.31f);
    cube.lightColor = lightColor;
    cube.ambientStrength = 0.9f;
    cube.diffuseStrength = 1.0f;
    cube.specularStrength = 0.2f;
    models.push_back(std::move(cube));
  }

  // Wall material properties.
  const float wallAmbient = 0.4f;
  const float wallDiffuse = 0.8f;
  const float wallSpecular = 0.4f;

  // Create front and back walls.
  Model backWall = Model(
        dg::Mesh::Quad,
        simpleTextureShader,
        brickTexture,
        glm::vec2(5, 2),
        Transform::TRS(
          glm::vec3(1, 1, -1.5),
          glm::quat(glm::radians(glm::vec3(0))),
          glm::vec3(5, 2, 1)
          ));
  backWall.lit = true;
  backWall.lightColor = lightColor;
  backWall.ambientStrength = wallAmbient;
  backWall.diffuseStrength = wallDiffuse;
  backWall.specularStrength = wallSpecular;
  Model frontWall = Model(backWall);
  frontWall.transform = frontWall.transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(0, 180, 0))));
  frontWall.transform.translation.z *= -1;
  models.push_back(std::move(backWall));
  models.push_back(std::move(frontWall));

  // Create left and right walls.
  Model leftWall = Model(
        dg::Mesh::Quad,
        simpleTextureShader,
        brickTexture,
        glm::vec2(3, 2),
        Transform::TRS(
          glm::vec3(-1.5f, 1, 0),
          glm::quat(glm::radians(glm::vec3(0, 90, 0))),
          glm::vec3(3, 2, 1)
          ));
  leftWall.lit = true;
  leftWall.lightColor = lightColor;
  leftWall.ambientStrength = wallAmbient;
  leftWall.diffuseStrength = wallDiffuse;
  leftWall.specularStrength = wallSpecular;
  Model rightWall = Model(leftWall);
  rightWall.transform = rightWall.transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(0, 180, 0))));
  rightWall.transform.translation.x = 3.5f;
  models.push_back(std::move(leftWall));
  models.push_back(std::move(rightWall));

  // Create floor and ceiling.
  Model floor = Model(
        dg::Mesh::Quad,
        simpleTextureShader,
        rustyPlateTexture,
        glm::vec2(5, 3) * 2.f,
        Transform::TRS(
          glm::vec3(1, 0, 0),
          glm::quat(glm::radians(glm::vec3(-90, 0, 0))),
          glm::vec3(5, 3, 1)
          ));
  floor.lit = true;
  floor.lightColor = lightColor;
  floor.ambientStrength = wallAmbient;
  floor.diffuseStrength = wallDiffuse;
  floor.specularStrength = wallSpecular;
  Model ceiling = Model(floor);
  ceiling.texture = hardwoodTexture;
  ceiling.transform = ceiling.transform * Transform::R(
      glm::quat(glm::radians(glm::vec3(180, 0, 0))));
  ceiling.transform.translation.y = 2;
  ceiling.specularStrength = 0.1f;
  ceiling.uvScale /= 2;
  models.push_back(std::move(floor));
  models.push_back(std::move(ceiling));

  // Create portal models.
  Model redPortalModel = Model(
      dg::Mesh::Quad,
      solidColorShader,
      portalTransforms[0] * portalQuadScale);
  redPortalModel.lit = true;
  redPortalModel.albedo = glm::vec3(1, 0, 0);
  redPortalModel.lightColor = lightColor;
  redPortalModel.ambientStrength = wallAmbient;
  redPortalModel.diffuseStrength = wallDiffuse;
  redPortalModel.specularStrength = 0;
  Model bluePortalModel = Model(redPortalModel);
  bluePortalModel.albedo = glm::vec3(0, 0, 1);
  bluePortalModel.transform = portalTransforms[1] * portalQuadScale;
  models.push_back(std::move(redPortalModel));
  models.push_back(std::move(bluePortalModel));

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
  for (auto model = models.begin(); model != models.end(); model++) {
    model->invPortal = invPortal;
    model->shader->SetVec3("LightPosition", xfLight.translation);
    model->shader->SetVec3("CameraPosition", view.Inverse().translation);
    model->Draw(view.ToMat4(), projection);
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

  solidColorShader->Use();
  solidColorShader->SetVec3("Albedo", backgroundColor);
  solidColorShader->SetMat4("InvPortal", glm::mat4x4(0));
  solidColorShader->SetMat4(
      "MATRIX_MVP", projection * view * xfPortal * portalOpeningScale);
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
