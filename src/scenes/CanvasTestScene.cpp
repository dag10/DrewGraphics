//
//  scenes/CanvasTestScene.h
//

#include <scenes/CanvasTestScene.h>

std::unique_ptr<dg::CanvasTestScene> dg::CanvasTestScene::Make() {
  return std::unique_ptr<dg::CanvasTestScene>(new dg::CanvasTestScene());
}

dg::CanvasTestScene::CanvasTestScene() : CanvasScene() {}

void dg::CanvasTestScene::Initialize() {
  CanvasScene::Initialize();

  // Create canvas.
  // TODO: Actual make this a Canvas.
  canvas = std::make_shared<Texture>(
      Texture::FromPath("assets/textures/container2.png"));
  quadMaterial->SetTexture(canvas);
}

