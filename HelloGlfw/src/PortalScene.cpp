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

#include <iostream>
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
  std::cout << "Scale: " << portalTransforms[0].scale.x << std::endl;
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

void dg::PortalScene::Render(dg::Window& window) {
  // Set up view.
  glm::mat4x4 view = camera.GetViewMatrix();
  glm::mat4x4 projection = camera.GetProjectionMatrix(
      window.GetWidth() / window.GetHeight());

  // Clear back buffer.
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render params.
  glEnable(GL_DEPTH_TEST);
  
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
  glCullFace(GL_FRONT_AND_BACK);

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

