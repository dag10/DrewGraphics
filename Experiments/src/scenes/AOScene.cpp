//
//  scenes/AOScene.cpp
//

#include "dg/scenes/AOScene.h"

#include <forward_list>
#include <glm/glm.hpp>
#include <random>
#include "dg/Camera.h"
#include "dg/Canvas.h"
#include "dg/Graphics.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/Shader.h"
#include "dg/Skybox.h"
#include "dg/Texture.h"
#include "dg/Utils.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/KeyboardLightController.h"
#include "dg/materials/DeferredMaterial.h"
#include "dg/materials/LightPassMaterial.h"
#include "dg/materials/SSAOMaterial.h"
#include "dg/materials/ScreenQuadMaterial.h"
#include "dg/materials/StandardMaterial.h"

std::unique_ptr<dg::AOScene> dg::AOScene::Make() {
  return std::unique_ptr<dg::AOScene>(new dg::AOScene());
}

dg::AOScene::AOScene() : Scene() {}

void dg::AOScene::Initialize() {
  Scene::Initialize();

  // Lock window cursor to center.
  window->LockCursor();

  // Create skybox.
  skybox =
      Skybox::Create(Texture::FromPath("assets/textures/skybox_daylight.png"));

  // Create primary sky light.
  skylights = std::make_shared<SceneObject>();
  AddChild(skylights);
  auto primarySkyLight = std::make_shared<DirectionalLight>(
      glm::vec3(1.0f, 0.93f, 0.86f), 0.4, 0.82, 0.07);
  primarySkyLight->LookAtDirection(glm::normalize(glm::vec3(-0.3f, -1, -0.2f)));
  //Behavior::Attach(primarySkyLight,
                   //std::make_shared<KeyboardLightController>(window));
  skylights->AddChild(primarySkyLight);

  // Create secondary sky light.
  auto secondarySkyLight = std::make_shared<DirectionalLight>(
      primarySkyLight->GetDiffuse(), 0, 0.21, 0);
  secondarySkyLight->LookAtDirection(-primarySkyLight->transform.Forward());
  //Behavior::Attach(secondarySkyLight,
                   //std::make_shared<KeyboardLightController>(window));
  skylights->AddChild(secondarySkyLight);

  // Create camera point light.
  pointlight =
      std::make_shared<PointLight>(primarySkyLight->GetDiffuse(), 0, 0.17, 0);
  cameras.main->AddChild(pointlight, false);
  Behavior::Attach(pointlight,
                   std::make_shared<KeyboardLightController>(window));

  // Stone material,
  DeferredMaterial floorMaterial =
      DeferredMaterial::WithTexture(Texture::FromPath(
          "assets/textures/Flooring_Stone_001/Flooring_Stone_001_COLOR.png"));
  floorMaterial.SetNormalMap(Texture::FromPath(
      "assets/textures/Flooring_Stone_001/Flooring_Stone_001_NRM.png"));
  floorMaterial.SetSpecular(Texture::FromPath(
      "assets/textures/Flooring_Stone_001/Flooring_Stone_001_SPEC.png"));
  floorMaterial.SetShininess(9);
  floorMaterial.SetUVScale(glm::vec2(5, 3) * 2.f);
  floorMaterial.SetLit(true);

  // Create a flashlight attached to the camera.
  flashlight = std::make_shared<SpotLight>(glm::vec3(1), 0.45f, 1.3f, 1.6f);
  flashlight->SetCutoff(glm::radians(25.f));
  flashlight->SetFeather(glm::radians(2.f));
  flashlight->SetCutoff(glm::radians(25.f));
  flashlight->SetCastShadows(true);
  Behavior::Attach(flashlight,
                   std::make_shared<KeyboardLightController>(window));
  flashlight->transform = Transform::T(glm::vec3(0.1f, -0.1f, 0));
  cameras.main->AddChild(flashlight, false);
  flashlight->enabled = false;

  // Load model.
  AddChild(std::make_shared<Model>(
      Mesh::LoadOBJ("assets/models/crytek-sponza/sponza.obj"),
      //std::make_shared<Material>(DeferredMaterial::WithColor(glm::vec3(0.5))),
      std::make_shared<Material>(floorMaterial),
      Transform::S(glm::vec3(0.0025))));

  // Configure camera.
  cameras.main->transform.translation = glm::vec3(-3.11, 1.75, 0.23);
  cameras.main->LookAtDirection(glm::vec3(0.9713, -0.198, -0.132));

  // Allow camera to be controller by the keyboard and mouse.
  Behavior::Attach(cameras.main,
                   std::make_shared<KeyboardCameraController>(window));

  // Create screen-space quad for rendering light pass of deferred geometry
  // buffer.
  finalRenderQuad = std::make_shared<Model>(
      Mesh::ScreenQuad, std::make_shared<LightPassMaterial>(), Transform());
  finalRenderQuad->layer = LayerMask::Overlay();
  finalRenderQuad->material->queue = RenderQueue::Overlay;
  AddChild(finalRenderQuad);

  // Create screen space quads to visualize internal textures.
  for (int i = 0; i < 3; i++) {
    auto quad = std::make_shared<Model>(
        Mesh::Quad,
        std::make_shared<ScreenQuadMaterial>(glm::vec3(0), glm::vec2(1)),
        Transform());
    quad->layer = LayerMask::Overlay();
    quad->material->queue = RenderQueue::Overlay + 1;
    overlayQuads.push_back(quad);
    AddChild(quad);
  }

  // Create custom shadow framebuffer with albedo map (for visualization
  // overlay).
  FrameBuffer::Options options;
  options.width = 2048;
  options.height = 2048;
  options.depthReadable = true;
  options.hasStencil = false;
  subrenders.light.framebuffer = FrameBuffer::Create(options);
  // Don't render overlays in the shadowmap.
  subrenders.light.layerMask = LayerMask::ALL() - LayerMask::Overlay();

  CreateSSAOBuffers();
  InitializeSSAO();

  CreateGBuffer();
  InitializeDeferred();
}

void dg::AOScene::InitializeDeferred() {
  geometrySubrender.outputType = Subrender::OutputType::MonoscopicWindow;
  geometrySubrender.renderSkybox = false;
  geometrySubrender.sendLights = false;
  // In the geometry pass, don't render overlays.
  geometrySubrender.layerMask = LayerMask::ALL() - LayerMask::Overlay();

  // In the lighting pass, only render the overlays.
  subrenders.main.layerMask = LayerMask::Overlay();
}

void dg::AOScene::InitializeSSAO() {
  ssaoSubrender.outputType = Subrender::OutputType::MonoscopicWindow;
  ssaoSubrender.drawType = Subrender::DrawType::Quad;
  ssaoSubrender.sendLights = false;
  ssaoSubrender.material = std::make_shared<SSAOMaterial>();

  // random floats between 0.0 - 1.0
  std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
  std::default_random_engine generator;

  const int numSamples = 64;
  for (unsigned int i = 0; i < numSamples; ++i) {
    // Sample is a vector in tangent space.
    glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, // x = [-1, 1]
                     randomFloats(generator) * 2.0 - 1.0, // y = [-1, 1]
                     randomFloats(generator));            // z = [ 0, 1]
    sample = glm::normalize(sample);

    sample *= randomFloats(generator);
    float scale = (float)i / (float)numSamples;
    scale = lerp(0.1f, 1.0f, scale * scale);
    sample *= scale;
    ssaoKernel.push_back(sample);
  }

  // Create a 4x4 texture of random rotation vectors to tile over the screen.
  ssaoNoise = std::make_shared<Canvas>(4, 4);
  for (unsigned int i = 0; i < ssaoNoise->GetWidth(); i++) {
    for (unsigned int j = 0; j < ssaoNoise->GetHeight(); j++) {
      ssaoNoise->SetPixel(i, j,
                          glm::vec3(randomFloats(generator) * 2.0 - 1.0,
                                    randomFloats(generator) * 2.0 - 1.0, 0.0f));
    }
  }
  ssaoNoise->Submit();
  std::static_pointer_cast<SSAOMaterial>(finalRenderQuad->material)
      ->SetNoiseTexture(ssaoNoise->GetTexture());
}

void dg::AOScene::Update() {
  Scene::Update();

  // G key switches to GBuffer overlay state.
  if (window->IsKeyJustPressed(Key::G)) {
    overlayState = OverlayState::GBuffer;
  }

  // L key switches to lighting overlay state.
  if (window->IsKeyJustPressed(Key::L)) {
    overlayState = OverlayState::Lighting;
  }

  // O key switches to ssao overlay state.
  if (window->IsKeyJustPressed(Key::O)) {
    overlayState = OverlayState::SSAO;
  }

  // N key switches to no overlay state.
  if (window->IsKeyJustPressed(Key::N)) {
    overlayState = OverlayState::None;
  }

  // If F is tapped, toggle flashlight.
  if (window->IsKeyJustPressed(Key::F)) {
    skybox->enabled = !skybox->enabled;
    skylights->enabled = !skylights->enabled;
    pointlight->enabled = !pointlight->enabled;
    flashlight->enabled = !flashlight->enabled;
  }

  // If Space is tapped, tag whether flashlight is pinned to camera transform.
  if (window->IsKeyJustPressed(Key::SPACE)) {
    if (flashlight->Parent() == this) {
      cameras.main->AddChild(flashlight, true);
    } else {
      AddChild(flashlight, true);
    }
  }


  // Update overlay quads based on overlay state.
  for (auto &quad : overlayQuads) {
    quad->enabled = false;
    std::static_pointer_cast<ScreenQuadMaterial>(quad->material)
        ->SetRedChannelOnly(false);
  }
  switch (overlayState) {
    case OverlayState::None:
      break;
    case OverlayState::GBuffer: {
      glm::vec2 quadScale =
          glm::vec2(2.f / 3.f) / glm::vec2(window->GetAspectRatio(), 1);

      overlayQuads[0]->enabled = true;
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetScale(quadScale);
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetOffset(glm::vec2(1 - quadScale.x * 0.5,
                                (1.f / 3.f) + quadScale.y * 0.5));
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetTexture(geometrySubrender.framebuffer->GetColorTexture(0));

      overlayQuads[1]->enabled = true;
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[1]->material)
          ->SetScale(quadScale);
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[1]->material)
          ->SetOffset(glm::vec2(1 - quadScale.x * 0.5,
                                -(1.f / 3.f) + quadScale.y * 0.5));
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[1]->material)
          ->SetTexture(geometrySubrender.framebuffer->GetColorTexture(1));

      overlayQuads[2]->enabled = true;
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[2]->material)
          ->SetScale(quadScale);
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[2]->material)
          ->SetOffset(glm::vec2(1 - quadScale.x * 0.5, -1 + quadScale.y * 0.5));
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[2]->material)
          ->SetTexture(geometrySubrender.framebuffer->GetColorTexture(2));
      break;
    };
    case OverlayState::Lighting: {
      glm::vec2 quadScale =
          glm::vec2(2.f / 3.f) / glm::vec2(window->GetAspectRatio(), 1);

      overlayQuads[0]->enabled = true;
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetScale(quadScale);
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetOffset(glm::vec2(1 - quadScale.x * 0.5,
                                (1.f / 3.f) + quadScale.y * 0.5));
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetTexture(subrenders.light.framebuffer->GetColorTexture());

      overlayQuads[1]->enabled = true;
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[1]->material)
          ->SetScale(quadScale);
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[1]->material)
          ->SetOffset(glm::vec2(1 - quadScale.x * 0.5,
                                -(1.f / 3.f) + quadScale.y * 0.5));
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[1]->material)
          ->SetTexture(subrenders.light.framebuffer->GetDepthTexture());
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[1]->material)
          ->SetRedChannelOnly(true);
      break;
    };
    case OverlayState::SSAO: {
      glm::vec2 quadScale =
          glm::vec2(2.f / 3.f) / glm::vec2(window->GetAspectRatio(), 1);

      overlayQuads[0]->enabled = true;
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetScale(quadScale);
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetOffset(glm::vec2(1 - quadScale.x * 0.5,
                                (1.f / 3.f) + quadScale.y * 0.5));
      std::static_pointer_cast<ScreenQuadMaterial>(overlayQuads[0]->material)
          ->SetTexture(ssaoSubrender.framebuffer->GetColorTexture());
      break;
    };
  }
}

void dg::AOScene::RenderFramebuffers() {
  // Create or resize framebuffers if needed.
  glm::vec2 windowSize = window->GetFramebufferSize();
  if (geometrySubrender.framebuffer == nullptr ||
      geometrySubrender.framebuffer->GetWidth() != windowSize.x ||
      geometrySubrender.framebuffer->GetHeight() != windowSize.y) {
    CreateGBuffer();
    CreateSSAOBuffers();
  }

  PerformSubrender(geometrySubrender);
  PerformSubrender(ssaoSubrender);
}

void dg::AOScene::CreateGBuffer() {
  glm::vec2 windowSize = window->GetFramebufferSize();

  FrameBuffer::Options opts;
  opts.width = windowSize.x;
  opts.height = windowSize.y;
  opts.depthReadable = true;
  opts.hasStencil = false;
  opts.textureOptions = std::vector<TextureOptions>(4);

  // First color texture is the albedo (rgb) and blend alpha (a).
  opts.textureOptions[0].width = opts.width;
  opts.textureOptions[0].height = opts.height;
  opts.textureOptions[0].type = TexturePixelType::BYTE;
  opts.textureOptions[0].wrap = TextureWrap::CLAMP_EDGE;

  // Second color texture is the world-space position (xyz), and whether
  // the fragment is to be lit at all (w == 1).
  opts.textureOptions[1].width = opts.width;
  opts.textureOptions[1].height = opts.height;
  opts.textureOptions[1].type = TexturePixelType::FLOAT;
  opts.textureOptions[1].wrap = TextureWrap::CLAMP_EDGE;

  // Third color texture is the world-space normal.
  opts.textureOptions[2].width = opts.width;
  opts.textureOptions[2].height = opts.height;
  opts.textureOptions[2].type = TexturePixelType::FLOAT;
  opts.textureOptions[2].wrap = TextureWrap::CLAMP_EDGE;

  // Fourth color texture is the specular reflection amount (rgb), and
  // shininess (a).
  opts.textureOptions[3].width = opts.width;
  opts.textureOptions[3].height = opts.height;
  opts.textureOptions[3].type = TexturePixelType::BYTE;
  opts.textureOptions[3].wrap = TextureWrap::CLAMP_EDGE;

  geometrySubrender.framebuffer = FrameBuffer::Create(opts);

  LinkGeometryToLight();
  LinkGeometryToSSAO();
}

void dg::AOScene::CreateSSAOBuffers() {
  glm::vec2 windowSize = window->GetFramebufferSize();

  FrameBuffer::Options opts;
  opts.width = windowSize.x;
  opts.height = windowSize.y;
  opts.depthReadable = true;
  opts.hasStencil = false;
  opts.hasColor = true;
  ssaoSubrender.framebuffer = FrameBuffer::Create(opts);

  LinkGeometryToSSAO();
}

void dg::AOScene::LinkGeometryToSSAO() {
  if (ssaoSubrender.material == nullptr ||
      geometrySubrender.framebuffer == nullptr) {
    return;
  }

  // Point material handling the ssao pass to the new g-buffer textures.
  std::static_pointer_cast<SSAOMaterial>(ssaoSubrender.material)
      ->SetPositionTexture(geometrySubrender.framebuffer->GetColorTexture(1));
  std::static_pointer_cast<SSAOMaterial>(ssaoSubrender.material)
      ->SetNormalTexture(geometrySubrender.framebuffer->GetColorTexture(2));
}

void dg::AOScene::LinkGeometryToLight() {
  if (finalRenderQuad == nullptr || geometrySubrender.framebuffer == nullptr) {
    return;
  }

  // Point material handling the lighting pass to the new g-buffer textures.
  std::static_pointer_cast<LightPassMaterial>(finalRenderQuad->material)
      ->SetAlbedoTexture(geometrySubrender.framebuffer->GetColorTexture(0));
  std::static_pointer_cast<LightPassMaterial>(finalRenderQuad->material)
      ->SetPositionTexture(geometrySubrender.framebuffer->GetColorTexture(1));
  std::static_pointer_cast<LightPassMaterial>(finalRenderQuad->material)
      ->SetNormalTexture(geometrySubrender.framebuffer->GetColorTexture(2));
  std::static_pointer_cast<LightPassMaterial>(finalRenderQuad->material)
      ->SetSpecularTexture(geometrySubrender.framebuffer->GetColorTexture(3));
  std::static_pointer_cast<LightPassMaterial>(finalRenderQuad->material)
      ->SetDepthTexture(geometrySubrender.framebuffer->GetDepthTexture());
}

void dg::AOScene::PreRender() {
  // Always render geometry pass with whichever camera we're about to do
  // the main render with.
  geometrySubrender.camera = subrenders.main.camera;

  // SSAO pass needs to know the camera view matrix to convert gbuffer positions
  // into view space.
  ssaoSubrender.camera = subrenders.main.camera;
}
