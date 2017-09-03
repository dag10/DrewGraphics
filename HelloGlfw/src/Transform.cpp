//
//  Transform.cpp
//

#include "Transform.h"

#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

dg::Transform dg::Transform::T(glm::vec3 translation) {
  Transform xf;
  xf.translation = translation;
  return xf;
}

dg::Transform dg::Transform::R(glm::quat rotation) {
  Transform xf;
  xf.rotation = rotation;
  return xf;
}

dg::Transform dg::Transform::S(glm::vec3 scale) {
  Transform xf;
  xf.scale = scale;
  return xf;
}

dg::Transform dg::Transform::TR(glm::vec3 translation, glm::quat rotation) {
  Transform xf;
  xf.translation = translation;
  xf.rotation = rotation;
  return xf;
}

dg::Transform dg::Transform::RS(glm::quat rotation, glm::vec3 scale) {
  Transform xf;
  xf.rotation = rotation;
  xf.scale = scale;
  return xf;
}

dg::Transform dg::Transform::TS(glm::vec3 translation, glm::vec3 scale) {
  Transform xf;
  xf.translation = translation;
  xf.scale = scale;
  return xf;
}

dg::Transform dg::Transform::TRS(
    glm::vec3 translation, glm::quat rotation, glm::vec3 scale) {
  Transform xf;
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

std::string dg::Transform::ToString() const {
  std::stringstream ss;

  ss << "T: ";
  ss << translation.x << ", ";
  ss << translation.y << ", ";
  ss << translation.z << std::endl;

  ss << "R: ";
  ss << rotation.w << ", ";
  ss << rotation.x << ", ";
  ss << rotation.y << ", ";
  ss << rotation.z << std::endl;

  glm::vec3 eulerRot = glm::eulerAngles(rotation);
  ss << "R (euler): ";
  ss << eulerRot.x << ", ";
  ss << eulerRot.y << ", ";
  ss << eulerRot.z << std::endl;

  ss << "S: ";
  ss << scale.x << ", ";
  ss << scale.y << ", ";
  ss << scale.z << std::endl;

  return ss.str();
}


