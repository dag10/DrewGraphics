//
//  CanvasScene.cpp
//

#include "dg/CanvasScene.h"
#include "dg/Canvas.h"
#include "dg/Graphics.h"
#include "dg/Mesh.h"
#include "dg/Window.h"
#include "dg/materials/ScreenQuadMaterial.h"

dg::CanvasScene::CanvasScene() : Scene() {}

dg::CanvasScene::~CanvasScene() {}

void dg::CanvasScene::Initialize() {
  Scene::Initialize();
  subrenders.main.rasterizerState.SetWriteDepth(false);
  subrenders.main.rasterizerState.SetDepthFunc(
      RasterizerState::DepthFunc::ALWAYS);

  canvas = std::make_shared<Canvas>(
    (unsigned int)window->GetWidth(),
    (unsigned int)window->GetHeight());
  quadMaterial = std::make_shared<ScreenQuadMaterial>(
    glm::vec3(0), glm::vec2(2));
}

void dg::CanvasScene::RenderFrame() {
  Graphics::Instance->PushRasterizerState(subrenders.main.rasterizerState);
  ClearBuffer();
  quadMaterial->SetTexture(canvas->GetTexture());
  quadMaterial->Use();
  Mesh::Quad->Draw();
  Graphics::Instance->PopRasterizerState();
}

