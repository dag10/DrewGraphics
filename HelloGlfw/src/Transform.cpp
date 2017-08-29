//
//  Transform.cpp
//

#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

const dg::Transform dg::Transform::Identity = dg::Transform::TRS(
  glm::vec3(0, 0, 0),
  glm::quat(1, 0, 0, 0),
  glm::vec3(1, 1, 1)
);

dg::Transform dg::Transform::T(glm::vec3 translation) {
  Transform xf = Identity;
  xf.translation = translation;
  return xf;
}

dg::Transform dg::Transform::R(glm::quat rotation) {
  Transform xf = Identity;
  xf.rotation = rotation;
  return xf;
}

dg::Transform dg::Transform::S(glm::vec3 scale) {
  Transform xf = Identity;
  xf.scale = scale;
  return xf;
}

dg::Transform dg::Transform::TR(glm::vec3 translation, glm::quat rotation) {
  Transform xf = Identity;
  xf.translation = translation;
  xf.rotation = rotation;
  return xf;
}

dg::Transform dg::Transform::RS(glm::quat rotation, glm::vec3 scale) {
  Transform xf = Identity;
  xf.rotation = rotation;
  xf.scale = scale;
  return xf;
}

dg::Transform dg::Transform::TS(glm::vec3 translation, glm::vec3 scale) {
  Transform xf = Identity;
  xf.translation = translation;
  xf.scale = scale;
  return xf;
}

dg::Transform dg::Transform::TRS(
    glm::vec3 translation, glm::quat rotation, glm::vec3 scale) {
  Transform xf = Identity;
  xf.translation = translation;
  xf.rotation = rotation;
  xf.scale = scale;
  return xf;
}

glm::mat4x4 dg::Transform::ToMat4() const {
  glm::mat4x4 r = glm::toMat4(rotation);
  glm::mat4x4 t = glm::translate(glm::mat4x4(1), translation);
  glm::mat4x4 s = glm::scale(glm::mat4x4(1), scale);
  return t * r * s;
}

