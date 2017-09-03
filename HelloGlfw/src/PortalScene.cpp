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

static const dg::Transform portalTransforms[] = {
  dg::Transform::TR(
      glm::vec3(0, 0, -0.5f),
      glm::quat(glm::radians(glm::vec3(0, 0, 0)))),
  dg::Transform::TR(
      glm::vec3(-1.5f, 0, 0),
      glm::quat(glm::radians(glm::vec3(0, 90, 0)))),
};

std::unique_ptr<dg::PortalScene> dg::PortalScene::Make() {
  return std::unique_ptr<dg::PortalScene>(new dg::PortalScene());
}

void dg::PortalScene::Initialize() {
  // Create meshes.
  cubeMesh = dg::Mesh::Cube;
  quadMesh = dg::Mesh::Quad;

  // Create shaders.
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
  camera.transform.translation.x += \
      0.2f * sin(glm::radians(glfwGetTime() * 90));
  camera.transform.translation.y += \
      0.05f * cos(glm::radians(glfwGetTime() * 90));
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
    glm::mat4x4 inPortalMat = inPortal.ToMat4();
    glm::mat4x4 outPortalMat = outPortal.ToMat4();

    // Flip out portal around.
    outPortalMat = outPortalMat * dg::Transform::R(
        glm::quat(glm::radians(glm::vec3(0, 180, 0)))).ToMat4();

    // Find delta between the two portals.
    glm::mat4x4 xfDelta = inPortalMat * glm::inverse(outPortalMat);

    view = view * xfDelta;
  }

  // Set up cube material.
  simpleTextureShader.Use();
  simpleTextureShader.SetTexture(0, "MainTex", crateTexture);

  // Render cubes.
  cubeMesh->Use();
  int numCubes = sizeof(cubePositions) / sizeof(cubePositions[0]);
  for (int i = 0; i < numCubes; i++) {
    glm::mat4x4 model = dg::Transform::TS(
        cubePositions[i],
        glm::vec3(0.5f)
        ).ToMat4();

    simpleTextureShader.SetMat4("MATRIX_MVP", projection * view * model);
    cubeMesh->Draw();
  }
  cubeMesh->FinishUsing();

  // Prepare to render portals.
  solidColorShader.Use();
  dg::Transform portalQuadScale = dg::Transform::S(glm::vec3(1, 1.5f, 1));
  quadMesh->Use();

  // Render first (red) portal.
  solidColorShader.SetVec3("Albedo", 255, 0, 0);
  solidColorShader.SetMat4(
      "MATRIX_MVP",
      projection * view * portalTransforms[0].ToMat4() *
      portalQuadScale.ToMat4());
  quadMesh->Draw();

  // Render second (blue) portal.
  solidColorShader.SetVec3("Albedo", 0, 0, 255);
  solidColorShader.SetMat4(
      "MATRIX_MVP",
      projection * view * portalTransforms[1].ToMat4() *
      portalQuadScale.ToMat4());
  quadMesh->Draw();

  quadMesh->FinishUsing();
}

void dg::PortalScene::Render(dg::Window& window) {
  // Clear back buffer.
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render params.
  glEnable(GL_DEPTH_TEST);
  
  // Render immediate scene.
  //RenderScene(window, false, dg::Transform(), dg::Transform());

  // Render scene through first (red) portal.
  //RenderScene(window, true, portalTransforms[0], portalTransforms[1]);

  // Render scene through second (blue) portal.
  //RenderScene(window, true, portalTransforms[1], portalTransforms[0]);

  // Render wither the immediate scene (no keys pressed), the scene
  // as seen through the first (red) portal (1 pressed), or the scene
  // as seen through the second (blue) portal (2 pressed).
  if(glfwGetKey(window.GetHandle(), GLFW_KEY_1) == GLFW_PRESS) {
    RenderScene(window, true, portalTransforms[0], portalTransforms[1]);
  } else if(glfwGetKey(window.GetHandle(), GLFW_KEY_2) == GLFW_PRESS) {
    RenderScene(window, true, portalTransforms[1], portalTransforms[0]);
  } else {
    RenderScene(window, false, dg::Transform(), dg::Transform());
  }
}

