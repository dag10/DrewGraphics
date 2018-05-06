//
//  behaviors/ShipBehavior.cpp
//

#include "cavr/behaviors/ShipBehavior.h"
#include "cavr/materials/IntersectionDownscaleMaterial.h"
#include "cavr/materials/SphereIntersectionMaterial.h"
#include "cavr/scenes/GameScene.h"
#include "dg/Camera.h"
#include "dg/EngineTime.h"
#include "dg/Graphics.h"
#include "dg/Material.h"
#include "dg/Model.h"
#include "dg/SceneObject.h"
#include "dg/materials/StandardMaterial.h"

cavr::ShipBehavior::ShipBehavior(std::shared_ptr<dg::Model> hullSphere)
    : hullSphere(hullSphere) {}

void cavr::ShipBehavior::Initialize() {
  dg::Behavior::Initialize();

  auto obj = GetSceneObject();

  // Create subrender for drawing any cave geometry inside of the ship hull onto
  // a texture for intersection testing.
  dg::Transform xfBoundingSphere = hullSphere->SceneSpace();
  intersectionSubrender.type =
      dg::Scene::Subrender::Type::MonoscopicFramebuffer;
  intersectionSubrender.renderSkybox = false;
  intersectionSubrender.camera = std::make_shared<dg::Camera>();
  intersectionSubrender.camera->projection =
      dg::Camera::Projection::Orthographic;
  intersectionSubrender.camera->farClip = xfBoundingSphere.scale.z * 0.5f;
  intersectionSubrender.camera->nearClip = xfBoundingSphere.scale.z * -0.5f;
  intersectionSubrender.camera->orthoWidth = xfBoundingSphere.scale.x;
  intersectionSubrender.camera->orthoHeight = xfBoundingSphere.scale.y;
  intersectionSubrender.layerMask = GameScene::LayerMask::CaveGeometry();
  intersectionSubrender.material =
      std::make_shared<cavr::SphereIntersectionMaterial>();
  intersectionSubrender.material->rasterizerOverride.SetCullMode(
      dg::RasterizerState::CullMode::OFF);
  std::static_pointer_cast<dg::StandardMaterial>(intersectionSubrender.material)
      ->SetLit(false);
  obj->AddChild(intersectionSubrender.camera, false);

  // Create intersection framebuffer for rendering cave intersection test.
  dg::FrameBuffer::Options options;
  options.width = options.height = 64;
  options.depthReadable = false;
  options.hasStencil = false;
  options.mipmap = true;
  options.interpolation = dg::TextureInterpolation::NEAREST;
  intersectionSubrender.framebuffer = dg::FrameBuffer::Create(options);

  // Create custom subrender for downscaling the intersection texture.
  intersectionDownscaleSubrender.type =
      dg::Scene::Subrender::Type::MonoscopicFramebuffer;
  intersectionDownscaleSubrender.drawScene = false;

  // Create intersection downscale framebuffer for processing the cave
  // intersection texture.
  dg::FrameBuffer::Options downscaleFrameBufferOpts;
  downscaleFrameBufferOpts.width = 1;
  downscaleFrameBufferOpts.height = 1;
  downscaleFrameBufferOpts.depthReadable = false;
  downscaleFrameBufferOpts.hasStencil = false;
  intersectionDownscaleSubrender.framebuffer =
      dg::FrameBuffer::Create(downscaleFrameBufferOpts);

  // Create texture for transferring 1px downscaled intersection texture from
  // GPU to CPU.
  dg::TextureOptions stagingTexOpts =
      intersectionDownscaleSubrender.framebuffer->GetColorTexture()
          ->GetOptions();
  stagingTexOpts.cpuReadable = true;
  stagingTexOpts.shaderReadable = false;
  intersectionReadStagingTexture = dg::Texture::Generate(stagingTexOpts);

  // Set up shader for downscaling intersection texture into a 1x1 pixel
  // average.
  intersectionDownscaleModel = std::make_shared<dg::Model>(
      dg::Mesh::Quad,
      std::make_shared<IntersectionDownscaleMaterial>(
          intersectionSubrender.framebuffer->GetColorTexture()),
      dg::Transform());
}

void cavr::ShipBehavior::Start() { dg::Behavior::Start(); }

void cavr::ShipBehavior::Update() {
  dg::Behavior::Update();

  // Change sphere color based on intersection.
  std::static_pointer_cast<dg::StandardMaterial>(hullSphere->material)
      ->SetDiffuse(intersectsCave ? glm::vec4(1, 0, 0, 0.4)
                                  : glm::vec4(1, 1, 1, 0.1));
}

void cavr::ShipBehavior::DrawIntersectionDownscale() {
  intersectionDownscaleModel->Draw(glm::mat4x4(1), glm::mat4x4(1));
}

void cavr::ShipBehavior::GenerateIntersectionMips() {
  intersectionSubrender.framebuffer->GetColorTexture()->GenerateMips();
}

void cavr::ShipBehavior::ReadIntersectionResults() {
#if defined(_DIRECTX)
  ID3D11Texture2D *downscaledTexture =
      intersectionDownscaleSubrender.framebuffer->GetColorTexture()
          ->GetTexture();
  ID3D11Texture2D *stagingTexture =
      intersectionReadStagingTexture->GetTexture();

  // Copy intersection pixel from downscaled texture to staging texture.
  dg::Graphics::Instance->context->CopyResource(stagingTexture,
                                                downscaledTexture);

  // Access the data on the GPU in the staging texture to read back its one
  // pixel.
  D3D11_MAPPED_SUBRESOURCE shadingTextureData;
  HRESULT hr = dg::Graphics::Instance->context->Map(
      stagingTexture, 0, D3D11_MAP_READ, NULL, &shadingTextureData);
  if (FAILED(hr)) {
    throw std::runtime_error("Failed to map intersection texture.");
  }

  BYTE *textureBytes = (BYTE *)shadingTextureData.pData;
  intersectsCave = textureBytes[0] != 0;
  dg::Graphics::Instance->context->Unmap(stagingTexture, 0);
#endif
}
