//
//  scenes/CaveTestScene.h
//

#pragma once

#include <memory>
#include <vector>
#include "dg/Scene.h"

namespace dg {
  class SceneObject;
  class Model;
  class Mesh;
  class Light;
  class StandardMaterial;
} // namespace dg

namespace cavr {

  class Tunnel {

    public:

      static const int VerticesPerRing = 12;

      class Knot {

        public:

          // All units are in cave-local space.

          Knot() = default;
          Knot(glm::vec3 position, glm::vec3 forward, float radius);

          inline const glm::vec3 &GetPosition() const { return position; }
          inline const glm::vec3 &GetRight() const { return xf[0]; }
          inline const glm::vec3 &GetUp() const { return xf[1]; }
          inline glm::vec3 GetForward() const { return -xf[2]; }
          inline const glm::mat3x3 &GetXF() const { return xf; }
          inline float GetRadius() const { return radius; }
          inline const glm::vec3 &GetVertexPosition(int index) const {
            return vertices[index];
          }

         private:

          glm::vec3 PositionForVertex(int index) const;

          glm::vec3 position = glm::vec3(0);
          glm::mat3x3 xf = glm::mat3x3(1);
          float radius = 0;
          glm::vec3 vertices[VerticesPerRing];
      };

      class Segment {

        public:

          Segment(std::shared_ptr<Knot> start, std::shared_ptr<Knot> end);

          void CreateMesh();

          inline std::shared_ptr<Knot> GetStartKnot() const { return knots[0]; }
          inline std::shared_ptr<Knot> GetEndKnot() const { return knots[1]; }
          inline std::shared_ptr<dg::Mesh> GetMesh() const { return mesh; }

        public:

          std::shared_ptr<Knot> knots[2];
          std::shared_ptr<dg::Mesh> mesh = nullptr;

      };

      std::vector<Segment> segments;
  };

  class CaveTestScene : public dg::Scene {

    public:

      static std::unique_ptr<CaveTestScene> Make();

      virtual void Initialize();
      virtual void Update();

    private:

      CaveTestScene();

      void CreateCave();
      std::shared_ptr<dg::SceneObject> CreateKnotVertexModel(
const Tunnel::Knot &knot) const;
      std::shared_ptr<dg::Model> CreateKnotModel(
          const Tunnel::Knot &knot) const;

      std::shared_ptr<dg::Light> skyLight;

      std::shared_ptr<dg::StandardMaterial> segmentMaterial;
      std::shared_ptr<dg::StandardMaterial> segmentWireframeMaterial;
      std::shared_ptr<dg::StandardMaterial> knotVertexMaterial;
      std::shared_ptr<dg::StandardMaterial> knotMaterial;

  }; // class CaveTestScene

} // namespace cavr
