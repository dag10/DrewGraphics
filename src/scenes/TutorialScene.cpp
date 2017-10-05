//
//  scenes/TutorialScene.h
//

#include <scenes/TutorialScene.h>

#include <glm/glm.hpp>
#include <EngineTime.h>
#include <materials/StandardMaterial.h>

std::unique_ptr<dg::TutorialScene> dg::TutorialScene::Make() {
  return std::unique_ptr<dg::TutorialScene>(new dg::TutorialScene());
}

void dg::TutorialScene::Initialize() {
}

void dg::TutorialScene::Update() {
}

void dg::TutorialScene::Render() {
  // Clear back buffer.
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

