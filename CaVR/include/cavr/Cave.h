//
//  Cave.h
//

#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include "dg/Mesh.h"
#include "dg/Transform.h"

namespace cavr {

  class CaveSegment {

    public:

      static const int VerticesPerRing = 24;

      class Knot {

        public:

          Knot() = default;
          Knot(glm::vec3 position, glm::vec3 forward, float radius,
               float curveSpeed);

          inline const glm::vec3 &GetPosition() const { return position; }
          inline const glm::vec3 &GetRight() const { return xf[0]; }
          inline const glm::vec3 &GetUp() const { return xf[1]; }
          inline glm::vec3 GetForward() const { return -xf[2]; }
          inline const glm::mat3x3 &GetXF() const { return xf; }
          inline float GetRadius() const { return radius; }
          inline float GetCurveSpeed() const { return curveSpeed; }
          inline const glm::vec3 &GetVertexPosition(int index) const {
            assert(!vertices.empty());
            return vertices[index];
          }

          void CreateVertices();

        private:

          glm::vec3 position = glm::vec3(0);
          glm::mat3x3 xf = glm::mat3x3(1);
          float radius = 0;
          float curveSpeed = 1;
          std::vector<glm::vec3> vertices;

      }; // class Knot

      class KnotSet {

        public:

          KnotSet() = default;
          KnotSet(const KnotSet&);

          KnotSet InterpolatedKnots() const;

          std::vector<std::shared_ptr<Knot>> knots;
          dg::Transform transform;

      }; // class KnotSet

      class Ring {

        public:

          Ring(std::shared_ptr<Knot> start, std::shared_ptr<Knot> end);

          void CreateMesh(std::vector<dg::Mesh::Triangle> &triangles,
                          int parity);

          inline std::shared_ptr<Knot> GetStartKnot() const { return knots[0]; }
          inline std::shared_ptr<Knot> GetEndKnot() const { return knots[1]; }

        public:

          std::shared_ptr<Knot> knots[2];

      }; // class Ring

      inline std::shared_ptr<dg::Mesh> GetMesh() const {
        assert(mesh != nullptr);
        return mesh;
      }

      CaveSegment() = default;
      CaveSegment(const KnotSet &knots);

    private:

      void CreateMesh();

      std::vector<Ring> rings;
      std::shared_ptr<dg::Mesh> mesh = nullptr;

  }; // class CaveSegment

} // namespace cavr
