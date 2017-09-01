//
//  TutorialScene.h
//

#include "TutorialScene.h"

#include <glm/glm.hpp>

static const glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
  glm::vec3( 2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3( 2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3( 1.3f, -2.0f, -2.5f),
  glm::vec3( 1.5f,  2.0f, -2.5f),
  glm::vec3( 1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f),
};

void dg::TutorialScene::Initialize() {
  // Create cube mesh.
  cube = dg::Mesh::Cube;

  // Create shader.
  shader = dg::Shader::FromFiles(
      "assets/shaders/tutorialshader.v.glsl",
      "assets/shaders/tutorialshader.f.glsl");

  // Create textures.
  containerTexture = dg::Texture::FromPath("assets/textures/container.jpg");
  awesomeFaceTexture = dg::Texture::FromPath("assets/textures/awesomeface.png");
}

void dg::TutorialScene::Update() {
  // Rotate camera around center, and tell it to look at origin.
  camera.transform.translation = \
        glm::quat(glm::radians(glm::vec3(0.f, glfwGetTime() * -40.f, 0.f))) *
        glm::vec3(0, 2 + 1 * sin(glm::radians(glfwGetTime()) * 50), -5);
  camera.LookAtPoint(glm::vec3(0));
}

void dg::TutorialScene::Render(dg::Window& window) {
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
  shader.Use();
  shader.SetFloat("ELAPSED_TIME", glfwGetTime());
  shader.SetTexture(0, "MainTex", containerTexture);
  shader.SetTexture(1, "SecondaryTex", awesomeFaceTexture);

  // Render cubes.
  cube->Use();
  int numCubes = sizeof(cubePositions) / sizeof(cubePositions[0]);
  for (int i = 0; i < numCubes; i++) {
    glm::mat4x4 model = dg::Transform::TRS(
        cubePositions[i],
        glm::quat(glm::radians(glm::vec3(
              glfwGetTime() * 90 + 15 * i, 20 * i, -10 * i))),
        glm::vec3(0.5f + 0.5f * ((float)i / (float)numCubes))
        ).ToMat4();

    shader.SetMat4("MATRIX_MVP", projection * view * model);
    cube->Draw();
  }
  cube->FinishUsing();
}

