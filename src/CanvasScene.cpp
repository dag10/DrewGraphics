//
//  CanvasScene.cpp
//

#include <Mesh.h>
#include <CanvasScene.h>

dg::CanvasScene::CanvasScene() : Scene() {}

void dg::CanvasScene::Initialize() {
  Scene::Initialize();

  canvas = std::make_shared<Canvas>(
    (unsigned int)window->GetWidth(),
    (unsigned int)window->GetHeight());
  quadMaterial = std::make_shared<ScreenQuadMaterial>();
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
  Mesh::Quad->Use();
  Mesh::Quad->Draw();
  Mesh::Quad->FinishUsing();
}

