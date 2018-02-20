//
//  CanvasScene.cpp
//

#include <CanvasScene.h>
#include <Mesh.h>
#include <Canvas.h>
#include <Window.h>
#include <materials/ScreenQuadMaterial.h>

dg::CanvasScene::CanvasScene() : Scene() {}

dg::CanvasScene::~CanvasScene() {}

void dg::CanvasScene::Initialize() {
  Scene::Initialize();

  canvas = std::make_shared<Canvas>(
    (unsigned int)window->GetWidth(),
    (unsigned int)window->GetHeight());
  quadMaterial = std::make_shared<ScreenQuadMaterial>(
    glm::vec3(0), glm::vec2(2));
}

void dg::CanvasScene::ConfigureBuffer() {
  glDisable(GL_DEPTH_TEST);
}

void dg::CanvasScene::RenderFrame() {
  window->ResetViewport();

  ClearBuffer();
  ConfigureBuffer();

  // In case the canvas is ever recreated.
  quadMaterial->SetTexture(canvas->GetTexture());

  quadMaterial->Use();
  Mesh::Quad->Draw();
}

