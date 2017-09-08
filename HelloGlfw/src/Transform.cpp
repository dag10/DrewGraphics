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

dg::Transform dg::Transform::Inverse() const {
  Transform xf;
  xf.scale = 1.f / this->scale;
  xf.rotation = glm::inverse(this->rotation);
  xf.translation = xf.rotation * (xf.scale * -1.f * this->translation);
  return xf;
}

dg::Transform dg::operator*(const dg::Transform& a, const dg::Transform& b) {
  Transform xf;
  xf.scale = a.scale * b.scale;
  xf.rotation = a.rotation * b.rotation;
  xf.translation = a.translation + (a.rotation * (a.scale * b.translation));
  return xf;
}

glm::mat4x4 dg::operator*(const glm::mat4x4& a, const dg::Transform& b) {
  return a * b.ToMat4();
}

glm::mat4x4 dg::operator*(const dg::Transform& a, const glm::mat4x4& b) {
  return a.ToMat4() * b;
}

glm::mat4x4 dg::Transform::ToMat4() const {
  glm::mat4x4 r = glm::toMat4(rotation);
  glm::mat4x4 t = glm::translate(glm::mat4x4(1), translation);
  glm::mat4x4 s = glm::scale(glm::mat4x4(1), scale);
  return t * r * s;
}

glm::vec3 dg::Transform::Right() const {
  return rotation * RIGHT;
}

glm::vec3 dg::Transform::Up() const {
  return rotation * UP;
}

glm::vec3 dg::Transform::Forward() const {
  return rotation * FORWARD;
}

std::string dg::Transform::ToString() const {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

std::ostream& dg::operator<<(std::ostream& os, const dg::Transform& xf) {
  os << "T: ";
  os << xf.translation.x << ", ";
  os << xf.translation.y << ", ";
  os << xf.translation.z << std::endl;

  os << "R (quat): ";
  os << xf.rotation.w << ", ";
  os << xf.rotation.x << ", ";
  os << xf.rotation.y << ", ";
  os << xf.rotation.z << std::endl;

  glm::vec3 eulerRot = glm::eulerAngles(xf.rotation);
  os << "R (euler): ";
  os << eulerRot.x << ", ";
  os << eulerRot.y << ", ";
  os << eulerRot.z << std::endl;

  os << "S: ";
  os << xf.scale.x << ", ";
  os << xf.scale.y << ", ";
  os << xf.scale.z;

  return os;
}

