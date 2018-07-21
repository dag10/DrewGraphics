//
//  scenes/CubemapScene.h
//

#include "dg/scenes/CubemapScene.h"
#include <forward_list>
#include <glm/glm.hpp>
#include <iostream>
#include "dg/Camera.h"
#include "dg/EngineTime.h"
#include "dg/Lights.h"
#include "dg/Mesh.h"
#include "dg/Model.h"
#include "dg/Shader.h"
#include "dg/Skybox.h"
#include "dg/Texture.h"
#include "dg/Window.h"
#include "dg/behaviors/KeyboardCameraController.h"
#include "dg/behaviors/RotateBehavior.h"
#include "dg/materials/CubemapMirrorMaterial.h"
#include "dg/materials/StandardMaterial.h"

std::unique_ptr<dg::CubemapScene> dg::CubemapScene::Make() {
  return std::unique_ptr<CubemapScene>(new CubemapScene(false));
}

std::unique_ptr<dg::CubemapScene> dg::CubemapScene::MakeVR() {
  return std::unique_ptr<CubemapScene>(new CubemapScene(true));
}

dg::CubemapScene::CubemapScene(bool enableVR) : Scene() {
  vr.requested = enableVR;
}

void dg::CubemapScene::Initialize() {
  Scene::Initialize();

  std::cout
      << "This scene is a demo of showing a cubemap on a material surface."
      << std::endl
      << std::endl;
  if (!vr.enabled) {
    std::cout << "Camera controls:" << std::endl
              << "  Mouse: Look around" << std::endl
              << "  W: Move forward" << std::endl
              << "  A: Move left" << std::endl
              << "  S: Move backward" << std::endl
              << "  D: Move right" << std::endl
              << "  Shift: Increase move speed" << std::endl
              << "  O: Toggle camera orbit" << std::endl
              << "  R: Reset camera to initial position" << std::endl
              << "  C: Print current camera pose" << std::endl
              << std::endl
              << "Press ESC or Q to release the cursor, and press "
                 "again to quit."
              << std::endl
              << std::endl;
  }

  // Create skybox.
  auto skyCubemap = Texture::FromPaths(
      "assets/textures/skybox/right.jpg", "assets/textures/skybox/left.jpg",
      "assets/textures/skybox/top.jpg", "assets/textures/skybox/bottom.jpg",
      "assets/textures/skybox/back.jpg", "assets/textures/skybox/front.jpg");
  skybox = Skybox::Create(skyCubemap);

  // Create shiny brick material.
  StandardMaterial brickMaterial = StandardMaterial::WithTexture(
      Texture::FromPath("assets/textures/brickwall.jpg"));
  brickMaterial.SetNormalMap(
      Texture::FromPath("assets/textures/brickwall_normal.jpg"));
  brickMaterial.SetSpecular(0.6f);
  brickMaterial.SetShininess(64);

      brickMaterial.SetLit(false);

  // Rotating container for surrounding meshes.
  auto surroundingObjects =
      std::make_shared<SceneObject>(Transform::T(0.25f * UP));
  AddChild(surroundingObjects);
  Behavior::Attach(surroundingObjects,
                   std::make_shared<RotateBehavior>(glm::radians(45.f)));

  // Create some objects to position around the pedestal.
  float objectScale = 0.4f;
  float distanceFromPedestal = 0.8f;
  std::vector<std::shared_ptr<SceneObject>> objectsToAdd = {
      std::make_shared<Model>(Mesh::Cube,
                              std::make_shared<StandardMaterial>(brickMaterial),
                              Transform::S(glm::vec3(objectScale * 0.8f))),
      std::make_shared<Model>(Mesh::Sphere,
                              std::make_shared<StandardMaterial>(brickMaterial),
                              Transform::S(glm::vec3(objectScale))),
      std::make_shared<Model>(Mesh::LoadOBJ("assets/models/helix.obj"),
                              std::make_shared<StandardMaterial>(brickMaterial),
                              Transform::S(glm::vec3(objectScale * 0.4f))),
  };
  float totalAngle = 0;
  for (auto &obj : objectsToAdd) {
    obj->transform.translation = glm::quat(glm::vec3(0, totalAngle, 0)) *
                                 glm::vec3(distanceFromPedestal, 0, 0);
    surroundingObjects->AddChild(obj, false);
    totalAngle += glm::radians(360.f) / objectsToAdd.size();
  }

  // Create floor material.
  const int floorSize = 2;
  StandardMaterial floorMaterial =
      StandardMaterial::WithTexture(Texture::FromPath(
          "assets/textures/Flooring_Stone_001/Flooring_Stone_001_COLOR.png"));
  floorMaterial.SetNormalMap(Texture::FromPath(
      "assets/textures/Flooring_Stone_001/Flooring_Stone_001_NRM.png"));
  floorMaterial.SetSpecular(Texture::FromPath(
      "assets/textures/Flooring_Stone_001/Flooring_Stone_001_SPEC.png"));
  floorMaterial.SetShininess(9);
  //floorMaterial.SetUVScale(glm::vec2((float)floorSize));
  floorMaterial.SetUVScale(glm::vec2((float)floorSize) * 0.1f); // TODO TMP   
  floorMaterial.SetLit(true);

  // Create floor plane.
  AddChild(std::make_shared<Model>(
      Mesh::Cube, std::make_shared<StandardMaterial>(floorMaterial),
      Transform::TS(glm::vec3(0, -floorSize / 2.f, 0),
                    glm::vec3(floorSize, floorSize, floorSize))));

  // Create cylindrical pedestal.
  float pedestalWidth = 0.5f;
  float pedestalHeight = 0.28f;
  AddChild(
      std::make_shared<Model>(
          Mesh::Cylinder, std::make_shared<StandardMaterial>(floorMaterial),
          Transform::TS(
              glm::vec3(0, (-pedestalWidth / 2.f) + pedestalHeight, 0),
              glm::vec3(pedestalWidth))),
      false);

  // Create reflection probe cubemap.
  const int fbSize = 512;
  TextureOptions reflectionTexOpts;
  reflectionTexOpts.type = TextureType::CUBEMAP;
  reflectionTexOpts.width = fbSize;
  reflectionTexOpts.height = fbSize;
  reflectionTexOpts.format = TexturePixelFormat::RGBA;
  reflectionTexOpts.pixelType = TexturePixelType::BYTE;
  reflectionTexOpts.interpolation = TextureInterpolation::LINEAR;
  reflectionTexOpts.wrap = TextureWrap::CLAMP_EDGE;
  reflectionTexOpts.mipmap = false;
  FrameBuffer::Options fbOpts;
  fbOpts.width = fbSize;
  fbOpts.height = fbSize;
  fbOpts.textureOptions.push_back(reflectionTexOpts);
  reflectionSubrender.framebuffer = FrameBuffer::Create(fbOpts);

  // Configure reflection probe subrender.
  reflectionSubrender.outputType = Subrender::OutputType::CubemapFramebuffer;
  reflectionSubrender.face = TextureFace::Right;
  reflectionSubrender.camera = std::make_shared<Camera>();
  reflectionSubrender.camera->fov = glm::radians(90.f);

  // Use geometry shader for main render.
  // TODO: TEMPORARY
  reflectionSubrender
  //subrenders.main
      .shaderReplacements[StandardMaterial::GetStaticShader().get()] =
      Shader::FromFiles("assets/shaders/standard.cubemap.v.glsl",
                        "assets/shaders/standard.cubemap.g.glsl",
                        "assets/shaders/standard.f.glsl");

  // Create reflective sphere material.
  auto sphereMaterial = std::make_shared<CubemapMirrorMaterial>(
      reflectionSubrender.framebuffer->GetColorTexture());
  //sphereMaterial->SetNormalMap(
      //Texture::FromPath("assets/textures/brickwall_normal.jpg"));
  //auto sphereMaterial = std::make_shared<StandardMaterial>(
      //StandardMaterial::WithTransparentColor(glm::vec4(0.5, 0.5, 0.5, 0.8)));

  // Add reflective cubemap sphere to top of pedestal.
  float sphereDiameter = pedestalWidth * 0.75f;
  float sphereFloatDistance = 0.08f;
  auto reflectiveSphere = std::make_shared<Model>(
      Mesh::Sphere, sphereMaterial,
      Transform::TS(
          glm::vec3(
              0, pedestalHeight + (sphereDiameter / 2.f) + sphereFloatDistance,
              0),
          glm::vec3(sphereDiameter)));
  AddChild(reflectiveSphere, false);
  reflectiveSphere->AddChild(reflectionSubrender.camera, false);

  // Create point light source inside sphere.
  reflectiveSphere->AddChild(
      std::make_shared<PointLight>(glm::vec3(1, 0.93, 0.86), 0.732f, 0.399f,
                                   0.968f),
      false);

  if (!vr.enabled) {
    // Lock window cursor to center.
    window->LockCursor();

    // Configure camera.
    cameras.main->transform.translation = glm::vec3(0, 0.76, 0.69);
    cameras.main->LookAtDirection(glm::normalize(glm::vec3(0, -0.33, -0.94)));

    // Parent camera by an empty object that can rotate around world origin
    // if orbit mode is enabled.
    auto cameraContainer = std::make_shared<SceneObject>();
    AddChild(cameraContainer);
    cameraContainer->AddChild(cameras.main, true);
    cameraRotateBehavior = Behavior::Attach(
        cameraContainer, std::make_shared<RotateBehavior>(glm::radians(-25.f)));

    // Allow camera to be controller by the keyboard and mouse.
    Behavior::Attach(cameras.main,
                     std::make_shared<KeyboardCameraController>(window));
  }
}

void dg::CubemapScene::Update() {
  Scene::Update();

  // When O is tapped, toggle camera orbit.
  if (window->IsKeyJustPressed(Key::O)) {
    cameraRotateBehavior->enabled = !cameraRotateBehavior->enabled;
  }

  // When R is tapped, reset camera orbit in addition to camera position.
  if (window->IsKeyJustPressed(Key::R)) {
    cameraRotateBehavior->GetSceneObject()->transform = Transform();
  }
}

void dg::CubemapScene::RenderFramebuffers() {
  PerformSubrender(reflectionSubrender);
}
