//
//  scenes/CaveTestScene.h
//

#pragma once

#include <memory>
#include <vector>
#include "dg/Mesh.h"
#include "dg/Scene.h"

namespace dg {
  class SceneObject;
  class Model;
  class DirectionalLight;
  class PointLight;
  class StandardMaterial;
} // namespace dg

namespace cavr {

  class Tunnel {

    public:

      static const int VerticesPerRing = 24;

      class Knot {

        public:

          // All units are in cave-local space.

          Knot() = default;
          Knot(glm::vec3 position, glm::vec3 forward, float radius,
               float curveSpeed);

          inline const glm::vec3 &GetPosition() const { return position; }
          inline const glm::vec3 &GetRight() const { return xf[0]; }
          inline const glm::vec3 &GetUp() const { return xf[1]; }
          inline glm::vec3 GetForward() const { return -xf[2]; }
          inline const glm::mat3x3 &GetXF() const { return xf; }
          inline float GetRadius() const { return radius; }
          inline const glm::vec3 &GetVertexPosition(int index) const {
            return vertices[index];
          }

          void CreateVertices();

          static std::vector<std::shared_ptr<Knot>> InterpolateKnots(
              const std::vector<std::shared_ptr<Knot>> &knots);

        private:

          glm::vec3 position = glm::vec3(0);
          glm::mat3x3 xf = glm::mat3x3(1);
          float radius = 0;
          float curveSpeed = 1;
          std::vector<glm::vec3> vertices;

      };

      class Segment {

        public:

          Segment(std::shared_ptr<Knot> start, std::shared_ptr<Knot> end);

          void CreateMesh(std::vector<dg::Mesh::Triangle> &triangles,
                          int parity);

          inline std::shared_ptr<Knot> GetStartKnot() const { return knots[0]; }
          inline std::shared_ptr<Knot> GetEndKnot() const { return knots[1]; }

        public:

          std::shared_ptr<Knot> knots[2];

      };

      inline std::shared_ptr<dg::Mesh> GetMesh() const {
        assert(mesh != nullptr);
        return mesh;
      }

      void AddSegment(const Segment &segment);
      void CreateMesh();

    private:

      std::vector<Segment> segments;
      std::shared_ptr<dg::Mesh> mesh = nullptr;

  };

  class CaveTestScene : public dg::Scene {

    public:

      static std::unique_ptr<CaveTestScene> Make();

      virtual void Initialize();
      virtual void Update();

    private:

      CaveTestScene();

      static std::vector<std::shared_ptr<Tunnel::Knot>> CreateFunnelKnots();
      static std::vector<std::shared_ptr<Tunnel::Knot>> CreateArcKnots();

      void CreateCave();
      std::shared_ptr<dg::SceneObject> CreateKnotVertexModels(
          const Tunnel::Knot &knot) const;
      std::shared_ptr<dg::SceneObject> CreateKnotModels(
          const Tunnel::Knot &knot) const;

      std::shared_ptr<dg::PointLight> controllerLight;
      std::shared_ptr<dg::DirectionalLight> skyLight;
      std::shared_ptr<dg::Model> floor;
      std::shared_ptr<dg::SceneObject> knots;

      std::shared_ptr<dg::StandardMaterial> segmentMaterial;
      std::shared_ptr<dg::StandardMaterial> segmentTransparentMaterial;
      std::shared_ptr<dg::StandardMaterial> segmentWireframeMaterial;
      std::shared_ptr<dg::StandardMaterial> knotVertexMaterial;
      std::shared_ptr<dg::StandardMaterial> knotMaterial;
      std::shared_ptr<dg::StandardMaterial> knotForwardMaterial;

  }; // class CaveTestScene

} // namespace cavr
