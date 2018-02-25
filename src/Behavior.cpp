//
//  Behavior.cpp
//

#include "dg/Behavior.h"
#include <stdexcept>
#include "dg/SceneObject.h"

void dg::Behavior::Attach_Impl(
    std::shared_ptr<dg::SceneObject> object,
    std::shared_ptr<dg::Behavior> behavior) {
  if (behavior->sceneObject.lock() != nullptr) {
    throw std::runtime_error(
        "Attempted to attach an already-attached Behavior with a SceneObject.");
  }

  behavior->sceneObject = object;
  object->AddBehavior(behavior);
  behavior->Initialize();
}

void dg::Behavior::Update() {
  if (!started) {
    Start();
    started = true;
  }
}

std::shared_ptr<dg::SceneObject> dg::Behavior::GetSceneObject() const {
  return sceneObject.lock();
}

