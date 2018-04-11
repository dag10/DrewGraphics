//
//  Cave.cpp
//

#include "cavr/Cave.h"

cavr::CaveSegment::Knot::Knot(glm::vec3 position, glm::vec3 forward,
                              float radius, float curveSpeed)
    : position(position), radius(radius), curveSpeed(curveSpeed) {
  glm::vec3 right = glm::normalize(glm::cross(forward, dg::UP));
  glm::vec3 up = glm::normalize(glm::cross(right, forward));
  xf = glm::mat3x3(right, up, -forward);
}

cavr::CaveSegment::KnotSet::KnotSet(const KnotSet &other) {
  this->knots = std::vector<std::shared_ptr<Knot>>(other.knots);
  this->transform = other.transform;
}

cavr::CaveSegment::KnotSet cavr::CaveSegment::KnotSet::InterpolatedKnots()
    const {
  if (knots.size() <= 1) {
    return KnotSet(*this);
  }

  // TODO: Calculate based on length.
  const int subdivisions = 80;

  std::vector<std::shared_ptr<Knot>> newKnots;
  const size_t numKnots = knots.size();
  for (size_t i = 0; i < numKnots - 1; i++) {
    std::shared_ptr<Knot> first = knots[i];
    std::shared_ptr<Knot> second = knots[i + 1];

    newKnots.push_back(first);

    if (subdivisions <= 1) {
      continue;
    }

    // Use Cubic Hermite Curves to interpolate the positions.
    // http://www.cubic.org/docs/hermite.htm
    std::vector<glm::vec3> positions(subdivisions-1);
    for (int t = 1; t <= subdivisions - 1; t++) {
      const float s = (float)t / (float)subdivisions;
      const float h1 = (2 * s * s * s) - (3 * s * s) + 1;
      const float h2 = (-2 * s * s * s) + (3 * s * s);
      const float h3 = (s * s * s) - (2 * s * s) + s;
      const float h4 = (s * s * s) - (s * s);
      const float s1 = first->GetCurveSpeed() * sqrt(first->GetRadius()) * 10.f;
      const float s2 =
          second->GetCurveSpeed() * sqrt(second->GetRadius()) * 10.f;
      const glm::vec3 p1 = first->GetPosition();
      const glm::vec3 p2 = second->GetPosition();
      const glm::vec3 t1 = first->GetForward() * s1;
      const glm::vec3 t2 = second->GetForward() * s2;
      positions[t - 1] = (h1 * p1) + (h2 * p2) + (h3 * t1) + (h4 * t2);
    }

    for (int t = 1; t <= subdivisions - 1; t++) {
      // Interpolate the tangents based on next and previous knot positions.
      glm::vec3 pos = positions[t - 1];
      glm::vec3 prev = (t == 1 ? first->GetPosition() : positions[t - 2]);
      glm::vec3 next =
          (t == (subdivisions - 1) ? second->GetPosition() : positions[t]);
      glm::vec3 t1 = glm::normalize(next - pos);
      glm::vec3 t2 = glm::normalize(pos - prev);
      glm::vec3 forward = (t1 + t2) * 0.5f;

      // Smoothstep the radius and curveSpeed based on next and previous knots.
      float ss = glm::smoothstep(0.f, 1.f, (float)(t) / (float)subdivisions);
      float radius = first->GetRadius() +
                     ((second->GetRadius() - first->GetRadius()) * ss);
      float curveSpeed =
          first->GetCurveSpeed() +
          ((second->GetCurveSpeed() - first->GetCurveSpeed()) * ss);

      newKnots.push_back(
          std::make_shared<Knot>(pos, forward, radius, curveSpeed));
    }
  }

  newKnots.push_back(knots.back());

  KnotSet newKnotSet;
  newKnotSet.knots = newKnots;
  newKnotSet.transform = transform;
  return newKnotSet;
}

cavr::CaveSegment::CaveSegment(const KnotSet &knots) : originalKnotSet(knots) {
  knotSet = knots.InterpolatedKnots();

  // Determine vertex positions for a ring of vertices around the knot.
  for (auto &knot : knotSet.knots) {
    knot->CreateVertices();
  }

  // Create mesh for cave segment.
  CreateMesh();
}

void cavr::CaveSegment::Knot::CreateVertices() {
  assert(vertices.empty());
  vertices = std::vector<glm::vec3>(VerticesPerRing);
  for (int i = 0; i < CaveSegment::VerticesPerRing; i++) {
    vertices[i] = position + glm::quat(xf) *
                                 glm::quat(glm::radians(glm::vec3(
                                     0, 0, i * 360 / VerticesPerRing))) *
                                 glm::vec3(radius, 0, 0);

    // Randomize the position a little bit to make it bumpy.
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    glm::vec3 dir = glm::normalize(vertices[i] - position);
    vertices[i] += (dir * r * radius * 0.1f);
  }
}

void cavr::CaveSegment::CreateMesh() {
  std::vector<dg::Mesh::Triangle> triangles;

  int parity = 0;
  size_t numKnots = knotSet.knots.size();
  for (size_t i = 0; i < numKnots - 1; i++) {
    CreateRingMesh(triangles, parity, *knotSet.knots[i], *knotSet.knots[i + 1]);
    parity = 1 - parity;
  }
  mesh = dg::Mesh::Create();
  for (auto &triangle : triangles) {
    triangle.CalculateFaceNormal();
    mesh->AddTriangle(triangle);
  }
  mesh->FinishBuilding();
}

void cavr::CaveSegment::CreateRingMesh(std::vector<dg::Mesh::Triangle> &triangles,
                                   int parity, const Knot &firstKnot,
                                   const Knot &secondKnot) {
  const int numTriangles = VerticesPerRing * 2;

  const Knot *knots[] = {
    &firstKnot,
    &secondKnot,
  };

  for (int i = 0; i < VerticesPerRing; i++) {
    int nextIdx = (i + 1) % VerticesPerRing;
    int a = parity;
    int b = 1 - a;
    dg::Vertex v1(knots[a]->GetVertexPosition(i));
    dg::Vertex v2(knots[b]->GetVertexPosition(i));
    dg::Vertex v3(knots[b]->GetVertexPosition(nextIdx));
    dg::Vertex v4(knots[a]->GetVertexPosition(nextIdx));

    auto winding =
        (parity == 1) ? dg::Mesh::Winding::CW : dg::Mesh::Winding::CCW;

    triangles.push_back(dg::Mesh::Triangle(v1, v2, v3, winding));
    triangles.push_back(dg::Mesh::Triangle(v1, v3, v4, winding));

    parity = 1 - parity;
  }
}
