//
//  CanvasScene.cpp
//

#include <Mesh.h>
#include <CanvasScene.h>

dg::CanvasScene::CanvasScene() : Scene() {}

void dg::CanvasScene::Initialize() {
  quadMaterial = std::make_shared<ScreenQuadMaterial>(canvas);
}

void dg::CanvasScene::ConfigureBuffer() {
  glDisable(GL_DEPTH_TEST);
}

void dg::CanvasScene::RenderFrame() {
  ClearBuffer();
  ConfigureBuffer();

  quadMaterial->Use();
  Mesh::Quad->Use();
  Mesh::Quad->Draw();
  Mesh::Quad->FinishUsing();
}

