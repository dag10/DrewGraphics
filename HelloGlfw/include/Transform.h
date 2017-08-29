//
//  Transform.h
//
#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace dg {
  static const glm::vec3 X_BASIS (1.f,  0.f,  0.f);
  static const glm::vec3 Y_BASIS (0.f,  1.f,  0.f);
  static const glm::vec3 Z_BASIS (0.f,  0.f,  1.f);
  static const glm::vec3 UP      (0.f,  1.f,  0.f);

  struct Transform {

      static const struct Transform Identity;

      static Transform T(glm::vec3 translation);
      static Transform R(glm::quat rotation);
      static Transform S(glm::vec3 scale);

      static Transform TR(glm::vec3 translation, glm::quat rotation);
      static Transform RS(glm::quat rotation, glm::vec3 scale);
      static Transform TS(glm::vec3 translation, glm::vec3 scale);

      static Transform TRS(
          glm::vec3 translation, glm::quat rotation, glm::vec3 scale);

      glm::vec3 translation;
      glm::quat rotation;
      glm::vec3 scale;

      glm::mat4x4 ToMat4() const;

  }; // struct Transform

} // namespace dg
