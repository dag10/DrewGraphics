//
//  scenes/CanvasTestScene.h
//

#include <scenes/CanvasTestScene.h>
#include <Canvas.h>

std::unique_ptr<dg::CanvasTestScene> dg::CanvasTestScene::Make() {
  return std::unique_ptr<dg::CanvasTestScene>(new dg::CanvasTestScene());
}

dg::CanvasTestScene::CanvasTestScene() : CanvasScene() {}

void dg::CanvasTestScene::Initialize() {
  CanvasScene::Initialize();

  canvas->SetPixel(10, 10, 255, 0, 0);
  canvas->SetPixel(11, 10, 0, 255, 0);
  canvas->SetPixel(10, 11, 0, 0, 255);
  canvas->SetPixel(11, 11, 255, 255, 255);
  canvas->Submit();
}

