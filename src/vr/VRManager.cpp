//
//  vr/VRManager.cpp
//

#include "dg/vr/VRManager.h"
#include <iostream>
#include "dg/Exceptions.h"
#include "dg/Mesh.h"
#include "dg/SceneObject.h"
#include "dg/vr/VRTrackedObject.h"
#include "dg/vr/VRUtils.h"

dg::VRManager *dg::VRManager::Instance = nullptr;

void dg::VRManager::Initialize() {
  Behavior::Initialize();

  if (Instance != nullptr) {
    throw std::runtime_error("More than one VRManager was initialized.");
  }

  Instance = this;
  StartOpenVR();
}

void dg::VRManager::StartOpenVR() {
  if (!vr::VR_IsRuntimeInstalled()) {
    throw std::runtime_error("Please install the OpenVR Runtime.");
  }

  if (!vr::VR_IsHmdPresent()) {
    throw std::runtime_error("No VR headset is detected.");
  }

  vr::HmdError hmdError;
  vrSystem = vr::VR_Init(
    &hmdError, vr::EVRApplicationType::VRApplication_Scene);
  if (vrSystem == nullptr) {
    throw OpenVRError(hmdError);
  }

  vrCompositor = vr::VRCompositor();
  vrRenderModels = vr::VRRenderModels();

  UpdateRenderModelList();

  CreateFramebuffers();
}

void dg::VRManager::CreateFramebuffers() {
    uint32_t vrWidth, vrHeight;
    vrSystem->GetRecommendedRenderTargetSize(&vrWidth, &vrHeight);

    leftFramebuffer = FrameBuffer::Create(vrWidth, vrHeight, false, true);
    rightFramebuffer = FrameBuffer::Create(vrWidth, vrHeight, false, true);
}

dg::VRManager::~VRManager() {
  Instance = nullptr;

  vr::VR_Shutdown();
  vrSystem = nullptr;
  vrCompositor = nullptr;
}

void dg::VRManager::ReadyToRender() {
  // Block until OpenVR's "running start" thinks we should begin our render
  // commands. This also loads in the tracked device poses for the upcoming
  // render and predicted poses for the upcoming frame.
  vrCompositor->WaitGetPoses(
    poses.data(), poses.size(), nextPoses.data(), nextPoses.size());
  UpdatePoses();
}

void dg::VRManager::RenderFinished() {
  // At this point poses still contains the render poses, and nextPoses
  // contains the gamePoses for the upcoming frame.
  // We'll swap poses with nextPoses to make poses be the poses relevant
  // for the next frame, and we'll re-parse the new poses to update
  // controller positions, etc.
  std::swap(poses, nextPoses);
  UpdatePoses();
}

void dg::VRManager::UpdatePoses() {
  // If we previously had a controller device index, and that device is no
  // longer connected, we'll next try to find another connected device to use.
  if (leftControllerIndex != -1 &&
    !poses[leftControllerIndex].bDeviceIsConnected) {
    leftControllerIndex = -1;
  }
  if (rightControllerIndex != -1 &&
    !poses[rightControllerIndex].bDeviceIsConnected) {
    rightControllerIndex = -1;
  }

  // Associate a device with the left controller if needed.
  if (leftControllerIndex == -1) {
    for (int i = 1; i < (int)poses.size(); i++) {
      if (!poses[i].bDeviceIsConnected) continue;
      if (vrSystem->GetTrackedDeviceClass(i) !=
        vr::TrackedDeviceClass_Controller) continue;
      if (i == rightControllerIndex) continue;

      leftControllerIndex = i;
      break;
    }
  }
  if (rightControllerIndex == -1) {
    for (int i = 1; i < (int)poses.size(); i++) {
      if (!poses[i].bDeviceIsConnected) continue;
      if (vrSystem->GetTrackedDeviceClass(i) !=
        vr::TrackedDeviceClass_Controller) continue;
      if (i == leftControllerIndex) continue;

      rightControllerIndex = i;
      break;
    }
  }

  // Update registered VRTrackedObject transforms.
  for (
    auto iter = trackedObjects.begin();
    iter != trackedObjects.end();
    iter++) {

    auto trackedObject = *iter;
    auto trackedSceneObject = trackedObject->GetSceneObject();

    if (trackedObject->role ==
      vr::ETrackedControllerRole::TrackedControllerRole_Invalid) {
      if (trackedObject->deviceIndex == -1) {
        continue;
      }
      trackedSceneObject->transform = Transform(OVR2GLM(
        poses[trackedObject->deviceIndex].mDeviceToAbsoluteTracking));
    } else {
      int index = (trackedObject->role ==
        vr::ETrackedControllerRole::TrackedControllerRole_LeftHand)
        ? leftControllerIndex
        : rightControllerIndex;

      trackedObject->deviceIndex = index;
      if (index == -1) {
        trackedSceneObject->enabled = false;
      } else {
        trackedSceneObject->enabled = true;
        trackedSceneObject->transform = Transform(OVR2GLM(
          poses[index].mDeviceToAbsoluteTracking));
      }
    }
  }
}

void dg::VRManager::UpdateRenderModelList() {
  uint32_t numModels = vrRenderModels->GetRenderModelCount();
  for (unsigned int i = 0; i < numModels; i++) {
    size_t size = vrRenderModels->GetRenderModelName(i, nullptr, 0);
    std::vector<char> nameBuff(size);
    vrRenderModels->GetRenderModelName(i, nameBuff.data(), size);
    std::string name(nameBuff.data());

    auto rmInfo = std::make_shared<RenderModelInfo>();
    rmInfo->renderModelIndex = i;
    if (renderModels.find(name) == renderModels.end()) {
      renderModels[name] = rmInfo;
    }
  }
}

void dg::VRManager::RegisterTrackedObject(VRTrackedObject *object) {
  trackedObjects.push_front(object);
}

void dg::VRManager::DeregisterTrackedObject(VRTrackedObject *object) {
  trackedObjects.remove(object);
}

std::shared_ptr<dg::FrameBuffer> dg::VRManager::GetFramebuffer(
  vr::EVREye eye) const {
  switch (eye) {
    case vr::EVREye::Eye_Left:  return leftFramebuffer;
    case vr::EVREye::Eye_Right: return rightFramebuffer;
    default:                    return nullptr;
  }
}

void dg::VRManager::SubmitFrame(vr::EVREye eye) {
  vr::Texture_t frameTexture;
  frameTexture.eColorSpace = vr::EColorSpace::ColorSpace_Auto;
#if defined(_OPENGL)
  frameTexture.eType = vr::ETextureType::TextureType_OpenGL;
  frameTexture.handle =
      (void *)(long)GetFramebuffer(eye)->GetColorTexture()->GetHandle();
#elif defined(_DIRECTX)
  frameTexture.eType = vr::ETextureType::TextureType_DirectX;
  frameTexture.handle = GetFramebuffer(eye)->GetColorTexture()->GetTexture();
#endif
  vrCompositor->Submit(eye, &frameTexture, nullptr,
                       vr::EVRSubmitFlags::Submit_Default);
}

std::shared_ptr<dg::Mesh> dg::VRManager::GetRenderModelMesh(
    const std::string& name) {
  std::shared_ptr<RenderModelInfo> info = nullptr;

  auto pair = renderModels.find(name);
  if (pair == renderModels.end()) {
    return nullptr;
  }
  info = pair->second;

  if (info->mesh != nullptr) {
    return info->mesh;
  }

  // Synchronously load render model data from OpenVR.
  if (info->data == nullptr) {
    vr::EVRRenderModelError err = vr::VRRenderModelError_Loading;
    do {
      err = vrRenderModels->LoadRenderModel_Async(
          name.c_str(), &info->data);
    } while (err == vr::VRRenderModelError_Loading);
    if (err != vr::VRRenderModelError_None) {
      return nullptr;
    }
  }

  // Create single mesh component.
  auto mesh = Mesh::Create();
  for (unsigned int i = 0; i < info->data->unTriangleCount; i++) {
    glm::vec3 positions[3];
    glm::vec3 normals[3];
    glm::vec2 texCoords[3];

    for (int j = 0; j < 3; j++) {
      auto vert = info->data->rVertexData[info->data->rIndexData[i * 3 + j]];
      positions[j] = OVR2GLM(vert.vPosition);
      normals[j] = OVR2GLM(vert.vNormal);
      texCoords[j] = { vert.rfTextureCoord[0], vert.rfTextureCoord[1] };
    }

    mesh->AddTriangle(
      Vertex(positions[0], normals[0], texCoords[0]),
      Vertex(positions[1], normals[1], texCoords[1]),
      Vertex(positions[2], normals[2], texCoords[2]),
      Mesh::Winding::CW);
  }
  mesh->FinishBuilding();
  info->mesh = mesh;
  return mesh;
}

std::shared_ptr<dg::Texture> dg::VRManager::GetRenderModelTexture(
    const std::string &name) {
  std::shared_ptr<RenderModelInfo> info = nullptr;

  auto pair = renderModels.find(name);
  if (pair == renderModels.end()) {
    return nullptr;
  }
  info = pair->second;

  if (info->texture != nullptr) {
    return info->texture;
  }

  // Synchronously load render model data from OpenVR.
  if (info->data == nullptr) {
    vr::EVRRenderModelError err = vr::VRRenderModelError_Loading;
    do {
      err = vrRenderModels->LoadRenderModel_Async(name.c_str(), &info->data);
    } while (err == vr::VRRenderModelError_Loading);
    if (err != vr::VRRenderModelError_None) {
      return nullptr;
    }
  }

  // Synchronously load texture data from OpenVR.
  vr::RenderModel_TextureMap_t *texdata;
  vr::EVRRenderModelError err = vr::VRRenderModelError_Loading;
  do {
    err = vrRenderModels->LoadTexture_Async(info->data->diffuseTextureId,
                                            &texdata);
  } while (err == vr::VRRenderModelError_Loading);
  if (err != vr::VRRenderModelError_None) {
    return nullptr;
  }

  // Create texture.
  TextureOptions texOpts;
  texOpts.width = texdata->unWidth;
  texOpts.height = texdata->unHeight;
  texOpts.mipmap = true;
  texOpts.format = TexturePixelFormat::RGBA;
  texOpts.type = TexturePixelType::BYTE;
  auto texture = Texture::Generate(texOpts);
  texture->UpdateData(texdata->rubTextureMapData, true);
  info->texture = texture;
  return texture;
}

std::shared_ptr<dg::Mesh> dg::VRManager::GetHiddenAreaMesh(vr::EVREye eye) {
  std::shared_ptr<Mesh> *hiddenAreaMesh = (eye == vr::EVREye::Eye_Left) \
    ? &leftHiddenAreaMesh
    : &rightHiddenAreaMesh;

  if (*hiddenAreaMesh == nullptr) {
    *hiddenAreaMesh = Mesh::Create();
    vr::HiddenAreaMesh_t mesh = vrSystem->GetHiddenAreaMesh(eye);
    for (unsigned int i = 0; i < mesh.unTriangleCount; i++) {
      int offset = i * 3;
      (*hiddenAreaMesh)->AddTriangle(
        Vertex({
            mesh.pVertexData[offset + 0].v[0],
            mesh.pVertexData[offset + 0].v[1],
            0.f
          }),
        Vertex({
            mesh.pVertexData[offset + 1].v[0],
            mesh.pVertexData[offset + 1].v[1],
            0.f
          }),
        Vertex({
            mesh.pVertexData[offset + 2].v[0],
            mesh.pVertexData[offset + 2].v[1],
            0.f
          }),

        // Actually unknown, doesn't matter, backface culling is disabled.
        Mesh::Winding::CW
      );
    }
    (*hiddenAreaMesh)->FinishBuilding();
  }

  return *hiddenAreaMesh;
}
