//
//  SceneObject.cpp
//

#include <SceneObject.h>

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>

dg::SceneObject::SceneObject(Transform transform) : transform(transform) {}

void dg::SceneObject::AddBehavior(std::shared_ptr<Behavior> behavior) {
  if (behavior->SceneObject().get() != this) {
    throw std::runtime_error(
        "Do not call SceneObject::AddBehavior() to attach a behavior. "
        "Use the static function Behavior::Attach() instead.");
  }

  behaviors.push_back(behavior);
}

void dg::SceneObject::UpdateBehaviors() {
  // Execute Update() for all behaviors.
  for (
      auto behavior = behaviors.begin();
      behavior != behaviors.end();
      behavior++) {
    if ((*behavior)->enabled) {
      (*behavior)->Update();
    }
  }
}

// Creates a copy of the object without copying its children, and without
// a parent.
dg::SceneObject::SceneObject(SceneObject& other) {
  // TODO: Clone behaviors.
  this->transform = other.transform;
  this->enabled = other.enabled;
}

dg::Transform dg::SceneObject::SceneSpace() const {
  if (parent == nullptr) {
    return transform;
  }

  return parent->SceneSpace() * transform;
}

void dg::SceneObject::SetSceneSpace(Transform transform) {
  if (parent == nullptr) {
    this->transform = transform;
  } else {
    this->transform = parent->SceneSpace().Inverse() * transform;
  }
}

void dg::SceneObject::AddChild(std::shared_ptr<SceneObject> child) {
  AddChild(child, true);
}

void dg::SceneObject::AddChild(
    std::shared_ptr<SceneObject> child, bool preserveSceneSpace) {
  if (child->parent == this) return;
  if (child->parent != nullptr) {
    child->parent->children.erase(child);
  }
  children.insert(child);
  child->SetParent(this, preserveSceneSpace);
}

void dg::SceneObject::RemoveChild(std::shared_ptr<SceneObject> child) {
  if (child->parent != this) return;
  children.erase(child);
  child->SetParent(nullptr, true);
}

dg::SceneObject *dg::SceneObject::Parent() const {
  return parent;
}

std::set<std::shared_ptr<dg::SceneObject>> &dg::SceneObject::Children() {
  return children;
}

void dg::SceneObject::LookAt(const SceneObject& object) {
  LookAtPoint(object.SceneSpace().translation);
}

void dg::SceneObject::LookAtDirection(glm::vec3 direction) {
  direction = glm::normalize(direction);

  // If we're told to look straight up or down, we have to pick a yaw.
  // Let's just default to looking at the global forward direction.
  // TODO: Retain the previous yaw in this case.
  if (direction.y == 1 || direction.y == -1) {
    direction = glm::normalize(glm::vec3(0, direction.y, -0.0001f));
  }

  glm::vec3 eulerOrientation(0);

  float horizontalLen = glm::length(glm::vec2(direction.x, direction.z));
  eulerOrientation.x = atan(direction.y / horizontalLen);

  eulerOrientation.y = -atan(direction.x / -direction.z);
  if (direction.z >= 0) {
    eulerOrientation.y += glm::pi<float>();
  }

  Transform xf_SS = SceneSpace();
  xf_SS.rotation = glm::quat(eulerOrientation);
  SetSceneSpace(xf_SS);
}

void dg::SceneObject::LookAtPoint(glm::vec3 target) {
  LookAtDirection(target - SceneSpace().translation);
}

void dg::SceneObject::OrientUpwards() {
  LookAtDirection(SceneSpace().Forward());
}

// Assumes it was already added as a child to the new parent, and removed
// as a child from its old parent. This only sets the current parent field and
// updates the local transform.
void dg::SceneObject::SetParent(
    SceneObject *parent, bool preserveSceneSpace) {
  if (preserveSceneSpace) {
    Transform xf_SS = SceneSpace();
    this->parent = parent;
    SetSceneSpace(xf_SS);
  } else {
    this->parent = parent;
  }
}

