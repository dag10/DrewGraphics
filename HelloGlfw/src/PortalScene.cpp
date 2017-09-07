//
//  PortalScene.h
//

#include "PortalScene.h"

#include <glm/glm.hpp>
#include "Transform.h"

static const glm::vec3 cubePositions[] = {
  glm::vec3(  0.0f,  0.0f,  0.0f ), 
  glm::vec3( -1.0f,  0.0f,  0.0f ), 
  glm::vec3(  1.0f,  0.0f,  0.0f ), 
};

static dg::Transform portalTransforms[] = {
  dg::Transform::TR(
      glm::vec3(0, 0, -0.5f),
      glm::quat(glm::radians(glm::vec3(0, 0, 0)))),
  dg::Transform::TR(
      glm::vec3(-1.5f, 0, 0),
      glm::quat(glm::radians(glm::vec3(0, 90, 0)))),
};

static const glm::vec3 backgroundColor = glm::vec3(0.2f, 0.3f, 0.3f);

static const dg::Transform portalQuadScale = \
    dg::Transform::S(glm::vec3(1, 1.5f, 1));
static const dg::Transform portalOpeningScale = \
    dg::Transform::TS(
        glm::vec3(0, 0, 0.0001f), // Prevent z-fighting between back and stencil.
        glm::vec3(
          portalQuadScale.scale.x - (0.02f * 2.f),
          portalQuadScale.scale.y - (0.02f * 2.f),
          1));

std::unique_ptr<dg::PortalScene> dg::PortalScene::Make() {
  return std::unique_ptr<dg::PortalScene>(new dg::PortalScene());
}

void dg::PortalScene::Initialize() {
  // Create meshes.
  cubeMesh = dg::Mesh::Cube;
  quadMesh = dg::Mesh::Quad;

  // Create shaders.
  dg::Shader::SetVertexHead("assets/shaders/includes/vertex_head.glsl");
  dg::Shader::AddVertexSource("assets/shaders/includes/vertex_main.glsl");
  dg::Shader::SetFragmentHead("assets/shaders/includes/fragment_head.glsl");
  dg::Shader::AddFragmentSource("assets/shaders/includes/fragment_main.glsl");
  depthResetShader = dg::Shader::FromFiles(
      "assets/shaders/depthreset.v.glsl",
      "assets/shaders/depthreset.f.glsl");
  simpleTextureShader = dg::Shader::FromFiles(
      "assets/shaders/simpletexture.v.glsl",
      "assets/shaders/simpletexture.f.glsl");
  solidColorShader = dg::Shader::FromFiles(
      "assets/shaders/solidcolor.v.glsl",
      "assets/shaders/solidcolor.f.glsl");

  // Create textures.
  crateTexture = dg::Texture::FromPath("assets/textures/container.jpg");
}

void dg::PortalScene::Update() {
  // Set camera position.
  camera.transform.translation = glm::vec3(0.9f, 0.6f, 2);
  camera.LookAtPoint(glm::vec3(0, 0, 0));

  // Wobble the camera.
  camera.transform.translation.x += \
      0.2f * sin(glm::radians(glfwGetTime() * 20));
  camera.transform.translation.y += \
      0.05f * cos(glm::radians(glfwGetTime() * 20));

  // Move the left portal in and out.
  portalTransforms[1].translation.x = \
      -1.5f - 0.15f + 0.3f * sin(glm::radians(glfwGetTime() * 90));
}

void dg::PortalScene::RenderScene(
    dg::Window& window, bool throughPortal,
    dg::Transform inPortal, dg::Transform outPortal) {

  // Set up view.
  glm::mat4x4 view = camera.GetViewMatrix();
  glm::mat4x4 projection = camera.GetProjectionMatrix(
      window.GetWidth() / window.GetHeight());

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

  // Set up cube material.
  simpleTextureShader.Use();
  simpleTextureShader.SetTexture(0, "MainTex", crateTexture);

  // Render cubes.
  cubeMesh->Use();
  int numCubes = sizeof(cubePositions) / sizeof(cubePositions[0]);
  for (int i = 0; i < numCubes; i++) {
    dg::Transform model = dg::Transform::TS(
        cubePositions[i],
        glm::vec3(0.5f));

    simpleTextureShader.SetMat4("MATRIX_MVP", projection * view * model);
    simpleTextureShader.SetMat4("MATRIX_M", model);
    if (throughPortal) {
      simpleTextureShader.SetMat4("InvPortal", outPortal.Inverse());
    } else {
      simpleTextureShader.SetMat4("InvPortal", glm::mat4x4(0));
    }
    cubeMesh->Draw();
  }
  cubeMesh->FinishUsing();

  // Prepare to render portals.
  solidColorShader.Use();
  quadMesh->Use();

  // Render first (red) portal back.
  solidColorShader.SetVec3("Albedo", 1, 0, 0);
  dg::Transform m = portalTransforms[0] * portalQuadScale;
  solidColorShader.SetMat4("MATRIX_MVP", projection * view * m);
  solidColorShader.SetMat4("MATRIX_M", m);
  solidColorShader.SetMat4("InvPortal", glm::mat4x4(0));
  quadMesh->Draw();

  // If maximum recursion reached, render a closed red portal.
  if (throughPortal) {
    solidColorShader.SetVec3("Albedo", 0.5f, 0, 0);
    dg::Transform m = portalTransforms[0] * portalOpeningScale;
    solidColorShader.SetMat4("MATRIX_MVP", projection * view * m);
    solidColorShader.SetMat4("MATRIX_M", m);
    solidColorShader.SetMat4("InvPortal", glm::mat4x4(0));
    quadMesh->Draw();
  }

  // Render second (blue) portal back;
  solidColorShader.SetVec3("Albedo", 0, 0, 1);
  m = portalTransforms[1] * portalQuadScale;
  solidColorShader.SetMat4("MATRIX_MVP", projection * view * m);
  solidColorShader.SetMat4("MATRIX_M", m);
  solidColorShader.SetMat4("InvPortal", glm::mat4x4(0));
  quadMesh->Draw();

  // If maximum recursion reached, render a closed blue portal.
  if (throughPortal) {
    solidColorShader.SetVec3("Albedo", 0, 0, 0.5f);
    dg::Transform m = portalTransforms[1] * portalOpeningScale;
    solidColorShader.SetMat4("MATRIX_MVP", projection * view * m);
    solidColorShader.SetMat4("MATRIX_M", m);
    solidColorShader.SetMat4("InvPortal", glm::mat4x4(0));
    quadMesh->Draw();
  }

  quadMesh->FinishUsing();
}

void dg::PortalScene::RenderPortalStencil(
    dg::Window& window, dg::Transform xfPortal) {
  glm::mat4x4 view = camera.GetViewMatrix();
  glm::mat4x4 projection = camera.GetProjectionMatrix(
      window.GetWidth() / window.GetHeight());

  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
  glClear(GL_STENCIL_BUFFER_BIT);

  solidColorShader.Use();
  solidColorShader.SetVec3("Albedo", backgroundColor);
  solidColorShader.SetMat4("InvPortal", glm::mat4x4(0));
  solidColorShader.SetMat4(
      "MATRIX_MVP", projection * view * xfPortal * portalOpeningScale);
  quadMesh->Use();
  quadMesh->Draw();
  quadMesh->FinishUsing();

  glDisable(GL_STENCIL_TEST);
}

void dg::PortalScene::ClearDepth(dg::Window& window) {
  glDepthFunc(GL_ALWAYS);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  depthResetShader.Use();
  quadMesh->Use();
  quadMesh->Draw();
  quadMesh->FinishUsing();

  glDepthFunc(GL_LESS);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void dg::PortalScene::Render(dg::Window& window) {
  // Clear back buffer.
  glClearColor(
      backgroundColor.x, backgroundColor.y, backgroundColor.z, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Render params.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  // Render immediate scene.
  RenderScene(window, false, dg::Transform(), dg::Transform());

  // Render first (red) portal stencil.
  RenderPortalStencil(window, portalTransforms[0]);

  // Render scene through first (red) portal.
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  ClearDepth(window); // Clear depth buffer only within stencil.
  RenderScene(window, true, portalTransforms[0], portalTransforms[1]);
  glDisable(GL_STENCIL_TEST);

  // Render first (red) portal stencil.
  RenderPortalStencil(window, portalTransforms[1]);

  // Render scene through second (blue) portal.
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  ClearDepth(window); // Clear depth buffer only within stencil.
  RenderScene(window, true, portalTransforms[1], portalTransforms[0]);
  glDisable(GL_STENCIL_TEST);
}

