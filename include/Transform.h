//
//  Transform.h
//
#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <ostream>

namespace dg {
  static const glm::vec3 X_BASIS ( 1,  0,  0 );
  static const glm::vec3 Y_BASIS ( 0,  1,  0 );
  static const glm::vec3 Z_BASIS ( 0,  0,  1 );
  static const glm::vec3 RIGHT   ( 1,  0,  0 );
  static const glm::vec3 UP      ( 0,  1,  0 );
  static const glm::vec3 FORWARD ( 0,  0, -1 );

  struct Transform {

      static Transform T(glm::vec3 translation);
      static Transform R(glm::quat rotation);
      static Transform S(glm::vec3 scale);

      static Transform TR(glm::vec3 translation, glm::quat rotation);
      static Transform RS(glm::quat rotation, glm::vec3 scale);
      static Transform TS(glm::vec3 translation, glm::vec3 scale);

      static Transform TRS(
          glm::vec3 translation, glm::quat rotation, glm::vec3 scale);

      // This constructor will only be accurate to the input matrix
      // if the matrix is an affine transformation.
      Transform(glm::mat4x4 mat);
      Transform() = default;

      glm::vec3 translation = glm::vec3(0, 0, 0);
      glm::quat rotation = glm::quat(1, 0, 0, 0);
      glm::vec3 scale = glm::vec3(1, 1, 1);

      Transform Inverse() const;
      friend Transform operator*(const Transform& a, const Transform& b);
      friend glm::mat4x4 operator*(const glm::mat4x4& a, const Transform& b);
      friend glm::mat4x4 operator*(const Transform& a, const glm::mat4x4& b);

      glm::mat4x4 ToMat4() const;

      glm::vec3 Right() const;
      glm::vec3 Up() const;
      glm::vec3 Forward() const;

      std::string ToString() const;
      friend std::ostream& operator<<(std::ostream& os, const dg::Transform& xf);

      bool operator==(const Transform& rhs) const;
      bool operator!=(const Transform& rhs) const;

  }; // struct Transform

} // namespace dg
