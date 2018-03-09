//
//  vr/VRRenderModel.cpp
//

#include "dg/vr/VRRenderModel.h"
#include <iostream>
#include "dg/EngineTime.h"
#include "dg/Model.h"
#include "dg/SceneObject.h"
#include "dg/materials/StandardMaterial.h"
#include "dg/vr/VRManager.h"
#include "dg/vr/VRTrackedObject.h"

void dg::VRRenderModel::Start() {
  trackedObject = GetSceneObject()->GetBehavior<VRTrackedObject>();
}

void dg::VRRenderModel::Update() {
  //auto trackedObject = this->trackedObject.lock();
  auto trackedObject = GetSceneObject()->GetBehavior<VRTrackedObject>();
  if (!trackedObject) {
    return;
  }

  if (Time::Elapsed < nextLoadRetryTime) {
    return;
  }

  int newDeviceIndex = trackedObject->deviceIndex;
  if (newDeviceIndex == deviceIndex) {
    return;
  }
  deviceIndex = newDeviceIndex;

  if (deviceIndex < 0) {
    DestroyRenderModel();
    return;
  }

  auto vrSystem = VRManager::Instance->vrSystem;
  char name[vr::k_unMaxPropertyStringSize];
  int len = vrSystem->GetStringTrackedDeviceProperty(
      deviceIndex, vr::Prop_RenderModelName_String, name, sizeof(name));
  if (len == 0) {
    std::cerr << "Failed to load render model name property for device "
              << deviceIndex << std::endl;
    nextLoadRetryTime = Time::Elapsed + 1.0;
    deviceIndex = -1;
    return;
  }

  LoadRenderModel(name);
}

void dg::VRRenderModel::DestroyRenderModel() {
  auto model = this->model.lock();
  if (!model) return;

  model->Parent()->RemoveChild(model);
}

void dg::VRRenderModel::LoadRenderModel(const std::string& modelName) {
  if (modelName == currentModelName && model.lock() != nullptr) {
    return;
  }
  currentModelName = modelName;

  VRManager::Instance->UpdateRenderModelList();

  StandardMaterial material = StandardMaterial::WithTexture(
      VRManager::Instance->GetRenderModelTexture(currentModelName));
  material.SetSpecular(0.3f);

  auto mesh = VRManager::Instance->GetRenderModelMesh(currentModelName);
  if (!mesh) {
    return;
  }

  DestroyRenderModel();

  std::shared_ptr<Model> model = std::make_shared<Model>(
      mesh, std::make_shared<StandardMaterial>(material), Transform());
  GetSceneObject()->AddChild(model, false);
  this->model = model;
}
