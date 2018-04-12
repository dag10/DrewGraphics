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
          Knot(const Knot &);
          Knot(dg::Transform xf, float curveSpeed);
          Knot(glm::vec3 position, glm::quat rotation, float radius,
               float curveSpeed);
          Knot(glm::vec3 position, glm::vec3 forward, float radius,
               float curveSpeed);

          inline const glm::vec3 &GetPosition() const { return xf.translation; }
          inline const glm::vec3 &GetRight() const { return xf.Right(); }
          inline const glm::vec3 &GetUp() const { return xf.Up(); }
          inline glm::vec3 GetForward() const { return xf.Forward(); }
          inline const dg::Transform &GetXF() const { return xf; }
          inline float GetRadius() const { return xf.scale.x; }
          inline float GetCurveSpeed() const { return curveSpeed; }
          inline int GetRotations() const { return rotations; }
          inline const glm::vec3 &GetVertexPosition(int index) const {
            assert(!vertices.empty());
            return vertices[index];
          }

          glm::quat GetUnrotatedRotation() const;

          void CreateVertices();

          void TransformBy(dg::Transform xf);
          void RotateBy(float approxRadians);

        private:

          dg::Transform xf;
          float curveSpeed = 1;
          int rotations = 0;
          std::vector<glm::vec3> vertices;

      }; // class Knot

      class KnotSet {

        public:

          // Create a copy whos knot pointers point to the original knots.
          static KnotSet RefCopy(const KnotSet &);

          // Create a copy whos knot pointers point to copies of the
          // original knots.
          static KnotSet FullCopy(const KnotSet &);

          KnotSet() = default;

          KnotSet WithInterpolatedKnots() const;
          KnotSet WithBakedTransform() const;
          KnotSet TransformedBy(dg::Transform xf) const;

          std::vector<std::shared_ptr<Knot>> noninterpolatedKnots;
          std::vector<std::shared_ptr<Knot>> knots;
          dg::Transform transform;

      }; // class KnotSet

      inline const KnotSet GetOriginalKnotSet() const {
        return originalKnotSet;
      }

      inline std::shared_ptr<dg::Mesh> GetMesh() const {
        assert(mesh != nullptr);
        return mesh;
      }

      CaveSegment() = default;
      CaveSegment(const KnotSet &knots);
      CaveSegment(const KnotSet &knots, const CaveSegment &previousSegment,
                  bool backwards = false);

     private:

      void CreateMesh();
      static void CreateRingMesh(std::vector<dg::Mesh::Triangle> &triangles,
                             int parity, const Knot &firstKnot,
                             const Knot &secondKnot);

      KnotSet originalKnotSet;
      std::shared_ptr<dg::Mesh> mesh = nullptr;

  }; // class CaveSegment

} // namespace cavr
