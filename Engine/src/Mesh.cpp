//
//  Mesh.cpp
//

#include "dg/Mesh.h"
#include <cassert>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include "dg/Exceptions.h"
#include "dg/Graphics.h"
#include "dg/Transform.h"

#ifdef _WIN32
#define sscanf sscanf_s
#endif

#pragma region Vertex

dg::Vertex::Vertex(glm::vec3 position) {
  data.position = position;
  attributes = AttrFlag::POSITION;
}

dg::Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord) {
  data.position = position;
  data.normal = normal;
  data.texCoord = texCoord;
  attributes = AttrFlag::POSITION | AttrFlag::NORMAL | AttrFlag::TEXCOORD;
}

dg::Vertex::Vertex(
    glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord,
    glm::vec3 tangent) {
  data.position = position;
  data.normal = normal;
  data.texCoord = texCoord;
  data.tangent = tangent;
  attributes = AttrFlag::POSITION | AttrFlag::NORMAL | AttrFlag::TEXCOORD |
               AttrFlag::TANGENT;
}

#pragma endregion
#pragma region Triangle

dg::Mesh::Triangle::Triangle(Vertex v1, Vertex v2, Vertex v3, Winding winding)
    : vertices{v1, v2, v3}, winding(winding) {
  if (v1.attributes != v2.attributes || v1.attributes != v3.attributes) {
    throw std::runtime_error(
        "Attempted to create a triangle out of vertices with mismatched "
        "attributes.");
  }
}

void dg::Mesh::Triangle::CalculateFaceNormal() {
  glm::vec3 d01 = vertices[1].data.position - vertices[0].data.position;
  glm::vec3 d02 = vertices[2].data.position - vertices[0].data.position;
  glm::vec3 normal = glm::normalize(glm::cross(d02, d01));

  if (winding == Mesh::Winding::CW) {
    normal *= -1;
  }

  Vertex::AttrFlag newAttrs = vertices[0].attributes | Vertex::AttrFlag::NORMAL;
  vertices[0].attributes = newAttrs;
  vertices[1].attributes = newAttrs;
  vertices[2].attributes = newAttrs;
  vertices[0].data.normal = normal;
  vertices[1].data.normal = normal;
  vertices[2].data.normal = normal;
}

#pragma endregion
#pragma region Base Class

dg::Mesh *dg::Mesh::lastDrawnMesh = nullptr;
std::unordered_map<std::string, std::weak_ptr<dg::Mesh>> dg::Mesh::fileMap;

std::shared_ptr<dg::Mesh> dg::Mesh::Cube = nullptr;
std::shared_ptr<dg::Mesh> dg::Mesh::MappedCube = nullptr;
std::shared_ptr<dg::Mesh> dg::Mesh::Quad = nullptr;
std::shared_ptr<dg::Mesh> dg::Mesh::ScreenQuad = nullptr;
std::shared_ptr<dg::Mesh> dg::Mesh::Cylinder = nullptr;
std::shared_ptr<dg::Mesh> dg::Mesh::Sphere = nullptr;

void dg::Mesh::CreatePrimitives() {
  assert(Mesh::Cube == nullptr);
  dg::Mesh::Cube = CreateCube();

  assert(Mesh::MappedCube == nullptr);
  dg::Mesh::MappedCube = CreateMappedCube();

  assert(Mesh::Quad == nullptr);
  dg::Mesh::Quad = CreateQuad();

  assert(Mesh::ScreenQuad == nullptr);
  dg::Mesh::ScreenQuad = CreateScreenQuad();

  assert(Mesh::Cylinder == nullptr);
  dg::Mesh::Cylinder = CreateCylinder(64, 1);

  assert(Mesh::Sphere == nullptr);
  dg::Mesh::Sphere = CreateSphere(32);
}

void dg::Mesh::AddQuad(
    Vertex v1, Vertex v2, Vertex v3, Vertex v4, Winding winding) {
  AddTriangle(v1, v2, v3, winding);
  AddTriangle(v1, v3, v4, winding);
}

void dg::Mesh::AddTriangle(Vertex v1, Vertex v2, Vertex v3, Winding winding) {
  AddTriangle(Triangle(v1, v2, v3, winding));
}

void dg::Mesh::AddTriangle(const Triangle &triangle) {
  using Flag = Vertex::AttrFlag;

  Vertex v1 = triangle.vertices[0];
  Vertex v2 = triangle.vertices[1];
  Vertex v3 = triangle.vertices[2];

  // TODO: Don't change winding here, but actually switch the rasterizer
  //       state to accept CCW winding.
#if defined(_OPENGL)
  Winding desiredWinding = Winding::CW;
#elif defined(_DIRECTX)
  Winding desiredWinding = Winding::CCW;
#endif

  if (triangle.winding != desiredWinding) {
    std::swap(v1, v2);
  }

  Vertex *v[] = { &v1, &v2, &v3 };

  // If we have positions and texture coordinates but no tangents, compute
  // the tangents.
  //
  // Adapted from http://www.terathon.com/code/tangent.html
  if (!!(v1.attributes & (Flag::POSITION | Flag::TEXCOORD)) &&
      !(v1.attributes & Flag::TANGENT)) {

    float x1 = v2.data.position.x - v1.data.position.x;
    float x2 = v3.data.position.x - v1.data.position.x;
    float y1 = v2.data.position.y - v1.data.position.y;
    float y2 = v3.data.position.y - v1.data.position.y;
    float z1 = v2.data.position.z - v1.data.position.z;
    float z2 = v3.data.position.z - v1.data.position.z;

    float s1 = v2.data.texCoord.x - v1.data.texCoord.x;
    float s2 = v3.data.texCoord.x - v1.data.texCoord.x;
    float t1 = v2.data.texCoord.y - v1.data.texCoord.y;
    float t2 = v3.data.texCoord.y - v1.data.texCoord.y;

    float r = 1.0F / (s1 * t2 - s2 * t1);
    glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
        (t2 * z1 - t1 * z2) * r);
    glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
        (s1 * z2 - s2 * z1) * r);

    for (long i = 0; i < 3; i++) {
      const glm::vec3& n = v[i]->data.normal;
      const glm::vec3& t = sdir;

      // Gram-Schmidt orthogonalize
      v[i]->data.tangent = glm::normalize(t - n * glm::dot(n, t));
      v[i]->attributes |= Vertex::AttrFlag::TANGENT;
    }
  }

  if (attributes == Flag::NONE) {
    attributes = v1.attributes;
  } else if (v1.attributes != attributes) {
    throw std::runtime_error(
        "Attempted to add a triangle to a mesh with noncompatible attributes.");
  }

  for (int i = 0; i < 3; i++) {
    auto hash = std::hash<Vertex>{}(*v[i]);
    auto pair = vertexMap.find(hash);
    unsigned int index = -1;
    if (pair == vertexMap.end()) {
      if (!!(attributes & Flag::POSITION)) {
        vertexPositions.push_back(v[i]->data.position);
      }
      if (!!(attributes & Flag::NORMAL)) {
        vertexNormals.push_back(v[i]->data.normal);
      }
      if (!!(attributes & Flag::TEXCOORD)) {
        vertexTexCoords.push_back(v[i]->data.texCoord);
      }
      if (!!(attributes & Flag::TANGENT)) {
        vertexTangents.push_back(v[i]->data.tangent);
      }
      index = (unsigned int)vertexPositions.size() - 1;
      vertexMap[hash] = index;
    } else {
      index = pair->second;
    }

    indices.push_back(index);
  }
}

const dg::Vertex dg::Mesh::GetVertex(int i) const {
  Vertex vertex(vertexPositions[i]);
  if (!!(attributes & Vertex::AttrFlag::NORMAL)) {
    vertex.data.normal = vertexNormals[i];
  }
  if (!!(attributes & Vertex::AttrFlag::TEXCOORD)) {
    vertex.data.texCoord = vertexTexCoords[i];
  }
  if (!!(attributes & Vertex::AttrFlag::TANGENT)) {
    vertex.data.tangent = vertexTangents[i];
  }
  vertex.attributes = attributes;
  return vertex;
}

void dg::Mesh::Draw() const {
  Graphics::Instance->ApplyCurrentRasterizerState();
}

std::shared_ptr<dg::Mesh> dg::Mesh::CreateCube() {
  std::shared_ptr<Mesh> mesh = Create();

  float S = 0.5f; // half size

  // Front
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, -S, +S }, { +0, +0, +1 }, { 0, 0 }, RIGHT },
    { { -S, +S, +S }, { +0, +0, +1 }, { 0, 1 }, RIGHT },
    { { +S, +S, +S }, { +0, +0, +1 }, { 1, 1 }, RIGHT },
    { { +S, -S, +S }, { +0, +0, +1 }, { 1, 0 }, RIGHT },
    Winding::CCW
  );

  // Back
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, -S, -S }, { +0, +0, -1 }, { 1, 0 }, -RIGHT },
    { { +S, -S, -S }, { +0, +0, -1 }, { 0, 0 }, -RIGHT },
    { { +S, +S, -S }, { +0, +0, -1 }, { 0, 1 }, -RIGHT },
    { { -S, +S, -S }, { +0, +0, -1 }, { 1, 1 }, -RIGHT },
    Winding::CCW
  );

  // Left
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, -S, -S }, { -1, +0, +0 }, { 0, 0 }, -FORWARD },
    { { -S, +S, -S }, { -1, +0, +0 }, { 0, 1 }, -FORWARD },
    { { -S, +S, +S }, { -1, +0, +0 }, { 1, 1 }, -FORWARD },
    { { -S, -S, +S }, { -1, +0, +0 }, { 1, 0 }, -FORWARD },
    Winding::CCW
  );

  // Right
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { +S, -S, +S }, { +1, +0, +0 }, { 0, 0 }, FORWARD },
    { { +S, +S, +S }, { +1, +0, +0 }, { 0, 1 }, FORWARD },
    { { +S, +S, -S }, { +1, +0, +0 }, { 1, 1 }, FORWARD },
    { { +S, -S, -S }, { +1, +0, +0 }, { 1, 0 }, FORWARD },
    Winding::CCW
  );

  // Top
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, +S, +S }, { +0, +1, +0 }, { 0, 0 }, RIGHT },
    { { -S, +S, -S }, { +0, +1, +0 }, { 0, 1 }, RIGHT },
    { { +S, +S, -S }, { +0, +1, +0 }, { 1, 1 }, RIGHT },
    { { +S, +S, +S }, { +0, +1, +0 }, { 1, 0 }, RIGHT },
    Winding::CCW
  );

  // Bottom
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, -S, -S }, { +0, -1, +0 }, { 0, 0 }, RIGHT },
    { { -S, -S, +S }, { +0, -1, +0 }, { 0, 1 }, RIGHT },
    { { +S, -S, +S }, { +0, -1, +0 }, { 1, 1 }, RIGHT },
    { { +S, -S, -S }, { +0, -1, +0 }, { 1, 0 }, RIGHT },
    Winding::CCW
  );

  mesh->FinishBuilding();

  return mesh;
}

std::shared_ptr<dg::Mesh> dg::Mesh::CreateMappedCube() {
  std::shared_ptr<Mesh> mesh = Create();

  const float S = 0.5f; // half size
  const float Q = 1.f / 4.f; // quater
  const float T = 1.f / 3.f; // third
  const float E = 1.f / (2048.f / 4.f * 3.f); // epsilon to hide seams

  const glm::vec2 BACK_TL   (Q*3+E, T*2-E);
  const glm::vec2 BACK_TR   (Q*4-E, T*2-E);
  const glm::vec2 BACK_BL   (Q*3+E, T*1+E);
  const glm::vec2 BACK_BR   (Q*4-E, T*1+E);

  const glm::vec2 FRONT_TL  (Q*1+E, T*2-E);
  const glm::vec2 FRONT_TR  (Q*2-E, T*2-E);
  const glm::vec2 FRONT_BL  (Q*1+E, T*1+E);
  const glm::vec2 FRONT_BR  (Q*2-E, T*1+E);

  const glm::vec2 LEFT_TL   (Q*0+E, T*2-E);
  const glm::vec2 LEFT_TR   (Q*1-E, T*2-E);
  const glm::vec2 LEFT_BL   (Q*0+E, T*1+E);
  const glm::vec2 LEFT_BR   (Q*1-E, T*1+E);

  const glm::vec2 RIGHT_TL  (Q*2+E, T*2-E);
  const glm::vec2 RIGHT_TR  (Q*3-E, T*2-E);
  const glm::vec2 RIGHT_BL  (Q*2+E, T*1+E);
  const glm::vec2 RIGHT_BR  (Q*3-E, T*1+E);

  const glm::vec2 BOTTOM_TL (Q*1+E, T*1-E);
  const glm::vec2 BOTTOM_TR (Q*2-E, T*1-E);
  const glm::vec2 BOTTOM_BL (Q*1+E, T*0+E);
  const glm::vec2 BOTTOM_BR (Q*2-E, T*0+E);

  const glm::vec2 TOP_TL    (Q*1+E, T*3-E);
  const glm::vec2 TOP_TR    (Q*2-E, T*3-E);
  const glm::vec2 TOP_BL    (Q*1+E, T*2+E);
  const glm::vec2 TOP_BR    (Q*2-E, T*2+E);

  // Front
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, -S, +S }, { +0, +0, +1 }, FRONT_BL, RIGHT },
    { { -S, +S, +S }, { +0, +0, +1 }, FRONT_TL, RIGHT },
    { { +S, +S, +S }, { +0, +0, +1 }, FRONT_TR, RIGHT },
    { { +S, -S, +S }, { +0, +0, +1 }, FRONT_BR, RIGHT },
    Winding::CCW
  );

  // Back
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, -S, -S }, { +0, +0, -1 }, BACK_BR,  -RIGHT },
    { { +S, -S, -S }, { +0, +0, -1 }, BACK_BL,  -RIGHT },
    { { +S, +S, -S }, { +0, +0, -1 }, BACK_TL,  -RIGHT },
    { { -S, +S, -S }, { +0, +0, -1 }, BACK_TR,  -RIGHT },
    Winding::CCW
  );

  // Left
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, -S, -S }, { -1, +0, +0 }, LEFT_BL,  -FORWARD },
    { { -S, +S, -S }, { -1, +0, +0 }, LEFT_TL,  -FORWARD },
    { { -S, +S, +S }, { -1, +0, +0 }, LEFT_TR,  -FORWARD },
    { { -S, -S, +S }, { -1, +0, +0 }, LEFT_BR,  -FORWARD },
    Winding::CCW
  );

  // Right
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { +S, -S, +S }, { +1, +0, +0 }, RIGHT_BL, FORWARD },
    { { +S, +S, +S }, { +1, +0, +0 }, RIGHT_TL, FORWARD },
    { { +S, +S, -S }, { +1, +0, +0 }, RIGHT_TR, FORWARD },
    { { +S, -S, -S }, { +1, +0, +0 }, RIGHT_BR, FORWARD },
    Winding::CCW
  );

  // Top
  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, +S, +S }, { +0, +1, +0 }, TOP_BL,   RIGHT },
    { { -S, +S, -S }, { +0, +1, +0 }, TOP_TL,   RIGHT },
    { { +S, +S, -S }, { +0, +1, +0 }, TOP_TR,   RIGHT },
    { { +S, +S, +S }, { +0, +1, +0 }, TOP_BR,   RIGHT },
    Winding::CCW
  );

  // Bottom
  mesh->AddQuad(
    // position       normal          texCoord   tangent
    { { -S, -S, -S }, { +0, -1, +0 }, BOTTOM_BL, RIGHT },
    { { -S, -S, +S }, { +0, -1, +0 }, BOTTOM_TL, RIGHT },
    { { +S, -S, +S }, { +0, -1, +0 }, BOTTOM_TR, RIGHT },
    { { +S, -S, -S }, { +0, -1, +0 }, BOTTOM_BR, RIGHT },
    Winding::CCW
  );

  mesh->FinishBuilding();

  return mesh;
}

std::shared_ptr<dg::Mesh> dg::Mesh::CreateQuad() {
  std::shared_ptr<Mesh> mesh = Create();

  float S = 0.5f; // half size

  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, -S, +0 }, { +0, +0, +1 }, { 0, 0 }, { 1, 0, 0 } },
    { { -S, +S, +0 }, { +0, +0, +1 }, { 0, 1 }, { 1, 0, 0 } },
    { { +S, +S, +0 }, { +0, +0, +1 }, { 1, 1 }, { 1, 0, 0 } },
    { { +S, -S, +0 }, { +0, +0, +1 }, { 1, 0 }, { 1, 0, 0 } },
    Winding::CCW
  );

  mesh->FinishBuilding();

  return mesh;
}

std::shared_ptr<dg::Mesh> dg::Mesh::CreateScreenQuad() {
  std::shared_ptr<Mesh> mesh = Create();

  float S = 1; // screen size

  mesh->AddQuad(
    // position       normal          texCoord  tangent
    { { -S, -S, +0 }, { +0, +0, +1 }, { 0, 0 }, { 1, 0, 0 } },
    { { -S, +S, +0 }, { +0, +0, +1 }, { 0, 1 }, { 1, 0, 0 } },
    { { +S, +S, +0 }, { +0, +0, +1 }, { 1, 1 }, { 1, 0, 0 } },
    { { +S, -S, +0 }, { +0, +0, +1 }, { 1, 0 }, { 1, 0, 0 } },
    Winding::CCW
  );

  mesh->FinishBuilding();

  return mesh;
}

std::shared_ptr<dg::Mesh> dg::Mesh::CreateCylinder(
    int radialDivisions, int heightDivisions) {
  std::shared_ptr<Mesh> mesh = Create();

  if (radialDivisions < 3) {
    radialDivisions = 3;
  }

  if (heightDivisions < 1) {
    heightDivisions = 1;
  }

  float halfHeight = 0.5f;
  float radInterval = glm::radians(360.f) / (float)radialDivisions;
  float radius = 0.5f;

  for (int i = 0; i < radialDivisions; i++) {
    glm::vec3 leftNormal = glm::quat(
        glm::vec3(0, radInterval * i, 0)) * -FORWARD;
    glm::vec3 rightNormal = glm::quat(
        glm::vec3(0, radInterval * (i + 1), 0)) * -FORWARD;

    glm::vec3 leftTangent = glm::quat(
        glm::vec3(0, radInterval * i, 0)) * RIGHT;
    glm::vec3 rightTangent = glm::quat(
        glm::vec3(0, radInterval * (i + 1), 0)) * RIGHT;

    glm::vec3 topLeft = (leftNormal * radius) + glm::vec3(0, halfHeight, 0);
    glm::vec3 topRight = (rightNormal * radius) + glm::vec3(0, halfHeight, 0);
    glm::vec3 bottomLeft = (leftNormal * radius) - glm::vec3(0, halfHeight, 0);
    glm::vec3 bottomRight =
      (rightNormal * radius) - glm::vec3(0, halfHeight, 0);

    glm::vec3 topCenter = glm::vec3(0, halfHeight, 0);
    glm::vec3 bottomCenter = glm::vec3(0, -halfHeight, 0);

    glm::vec2 uvTopCenter = glm::vec2(1.f / 6, 5.f / 6);
    float uvExtents = 1.f / 6;
    glm::vec2 uvBottomCenter = uvTopCenter;
    uvBottomCenter.y = 1 - uvBottomCenter.y;

    // Add top triangle.
    mesh->AddTriangle(
        {
          topLeft,
          UP,
          uvTopCenter + uvExtents * glm::vec2(-leftNormal.x, leftNormal.z),
          -RIGHT,
        },
        {
          topCenter,
          UP,
          uvTopCenter,
          -RIGHT,
        },
        {
          topRight,
          UP,
          uvTopCenter + uvExtents * glm::vec2(-rightNormal.x, rightNormal.z),
          -RIGHT,
        },
        Winding::CCW);

    // Add bottom triangle.
    mesh->AddTriangle(
        {
          bottomRight,
          -UP,
          uvBottomCenter - uvExtents * glm::vec2(rightNormal.x, rightNormal.z),
          -RIGHT,
        },
        {
          bottomCenter,
          -UP,
          uvBottomCenter,
          -RIGHT,
        },
        {
          bottomLeft,
          -UP,
          uvBottomCenter - uvExtents * glm::vec2(leftNormal.x, leftNormal.z),
          -RIGHT,
        },
        Winding::CCW);

    // Add side quad(s).
    float heightInterval = halfHeight * 2.f / heightDivisions;
    for (int j = 0; j < heightDivisions; j++) {
      glm::vec3 quadBottomLeft(
          bottomLeft + (j * heightInterval * UP));
      glm::vec3 quadBottomRight(
          bottomRight + (j * heightInterval * UP));
      glm::vec3 quadTopLeft(
          bottomLeft + ((j + 1) * heightInterval * UP));
      glm::vec3 quadTopRight(
          bottomRight + ((j + 1) * heightInterval * UP));

      float uvMinHeight = 1.f/3;
      float uvMaxHeight = 2.f/3;
      float uvHeightInterval = (uvMaxHeight - uvMinHeight) / heightDivisions;
      float uvBottomHeight = uvMinHeight + (uvHeightInterval * j);
      float uvTopHeight = uvMinHeight + (uvHeightInterval * (j + 1));
      glm::vec2 uvBottomLeft((float)i / radialDivisions, uvBottomHeight);
      glm::vec2 uvBottomRight((float)(i + 1) / radialDivisions, uvBottomHeight);
      glm::vec2 uvTopLeft((float)i / radialDivisions, uvTopHeight);
      glm::vec2 uvTopRight((float)(i + 1) / radialDivisions, uvTopHeight);

      mesh->AddQuad(
          {
            quadBottomLeft,  // Position
            leftNormal,      // Normal
            uvBottomLeft,    // Texture coordinate
            leftTangent,     // Tangent
          },
          {
            quadTopLeft,     // Position
            leftNormal,      // Normal
            uvTopLeft,       // Texture coordinate
            leftTangent,     // Tangent
          },
          {
            quadTopRight,    // Position
            rightNormal,     // Normal
            uvTopRight,      // Texture coordinate
            rightTangent,    // Tangent
          },
          {
            quadBottomRight, // Position
            rightNormal,     // Normal
            uvBottomRight,   // Texture coordinate
            rightTangent,    // Tangent
          },
          Winding::CCW);
    }
  }

  mesh->FinishBuilding();

  return mesh;
}

std::shared_ptr<dg::Mesh> dg::Mesh::CreateSphere(int subdivisions) {
  std::shared_ptr<Mesh> mesh = Create();

  if (subdivisions < 3) {
    subdivisions = 3;
  }

  float radInterval = glm::radians(360.f) / (float)subdivisions;
  float radius = 0.5f;

  for (int i = 0; i < subdivisions; i++) {
    glm::quat leftLongitudeQuat(glm::vec3(0, radInterval * i, 0));
    glm::quat rightLongitudeQuat(glm::vec3(0, radInterval * (i + 1), 0));

    float latInterval = glm::radians(180.f) / subdivisions;
    for (int j = 0; j < subdivisions; j++) {
      glm::quat bottomLatitudeQuat(
          glm::vec3(glm::radians(90.f) - (latInterval * j), 0, 0));
      glm::quat topLatitudeQuat(
          glm::vec3(glm::radians(90.f) - (latInterval * (j + 1)), 0, 0));

      glm::vec3 unit = -FORWARD * radius;
      glm::vec3 bottomLeft = leftLongitudeQuat * bottomLatitudeQuat * unit;
      glm::vec3 bottomRight = rightLongitudeQuat * bottomLatitudeQuat * unit;
      glm::vec3 topLeft = leftLongitudeQuat * topLatitudeQuat * unit;
      glm::vec3 topRight = rightLongitudeQuat * topLatitudeQuat * unit;

      glm::vec3 tangent = RIGHT;
      glm::vec3 bottomLeftTangent =
        leftLongitudeQuat * bottomLatitudeQuat * tangent;
      glm::vec3 bottomRightTangent =
        rightLongitudeQuat * bottomLatitudeQuat * tangent;
      glm::vec3 topLeftTangent =
        leftLongitudeQuat * topLatitudeQuat * tangent;
      glm::vec3 topRightTangent =
        rightLongitudeQuat * topLatitudeQuat * tangent;

      float uvHeightInterval = 1.f / subdivisions;
      float uvBottomHeight = uvHeightInterval * j;
      float uvTopHeight = uvHeightInterval * (j + 1);
      glm::vec2 uvBottomLeft((float)i / subdivisions, uvBottomHeight);
      glm::vec2 uvBottomRight((float)(i + 1) / subdivisions, uvBottomHeight);
      glm::vec2 uvTopLeft((float)i / subdivisions, uvTopHeight);
      glm::vec2 uvTopRight((float)(i + 1) / subdivisions, uvTopHeight);

      mesh->AddQuad(
          {
            bottomLeft,                  // Position
            glm::normalize(bottomLeft),  // Normal (the normalized position)
            uvBottomLeft,                // Texture coordinate
            bottomLeftTangent,           // Tangent
          },
          {
            topLeft,                     // Position
            glm::normalize(topLeft),     // Normal (the normalized position)
            uvTopLeft,                   // Texture coordinate
            topLeftTangent,              // Tangent
          },
          {
            topRight,                    // Position
            glm::normalize(topRight),    // Normal (the normalized position)
            uvTopRight,                  // Texture coordinate
            topRightTangent,             // Tangent
          },
          {
            bottomRight,                 // Position
            glm::normalize(bottomRight), // Normal (the normalized position)
            uvBottomRight,               // Texture coordinate
            bottomRightTangent,          // Tangent
          },
          Winding::CCW);
    }
  }

  mesh->FinishBuilding();

  return mesh;
}

std::shared_ptr<dg::Mesh> dg::Mesh::Create() {
#if defined(_OPENGL)
  return std::shared_ptr<Mesh>(new OpenGLMesh());
#elif defined(_DIRECTX)
  return std::shared_ptr<Mesh>(new DirectXMesh());
#endif
}

std::shared_ptr<dg::Mesh> dg::Mesh::LoadOBJ(const char *filename) {
  auto found = fileMap.find(filename);
  if (found != fileMap.end()) {
    std::shared_ptr<Mesh> mesh = found->second.lock();
    if (mesh == nullptr) {
      fileMap.erase(filename);
    } else {
      return mesh;
    }
  }

  std::shared_ptr<Mesh> mesh = Create();

  std::ifstream obj(filename, std::ifstream::binary);

  if (!obj.is_open()) {
    throw FileNotFoundException(filename);
  }

  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uvs;
  std::string line;

  while (obj.good()) {
    std::getline(obj, line);

    if (line.empty()) continue;

    if (line.at(0) == 'v' && line.at(1) == 'n') {
      glm::vec3 norm;
      sscanf(
          line.c_str(),
          "vn %f %f %f",
          &norm.x, &norm.y, &norm.z);
      normals.push_back(norm);
    } else if (line.at(0) == 'v' && line.at(1) == 't') {
      glm::vec2 uv;
      sscanf(
          line.c_str(),
          "vt %f %f",
          &uv.x, &uv.y);
      uvs.push_back(uv);
    } else if (line.at(0) == 'v') {
      glm::vec3 pos;
      sscanf(
          line.c_str(),
          "v %f %f %f",
          &pos.x, &pos.y, &pos.z);
      positions.push_back(pos);
    } else if (line.at(0) == 'f') {
      // Read the face indices into an array.
      unsigned int i[12];
      int facesRead = sscanf(
          line.c_str(),
          "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
          &i[0], &i[1], &i[2],
          &i[3], &i[4], &i[5],
          &i[6], &i[7], &i[8],
          &i[9], &i[10], &i[11]);

      // OBJ file indices are 1-based.
      Vertex v1(
          positions[i[0] - 1],
          normals[i[2] - 1],
          uvs[i[1] - 1]);
      Vertex v2(
          positions[i[3] - 1],
          normals[i[5] - 1],
          uvs[i[4] - 1]);
      Vertex v3(
          positions[i[6] - 1],
          normals[i[8] - 1],
          uvs[i[7] - 1]);

      // Add triangle.
      mesh->AddTriangle(v1, v2, v3, Winding::CW);

      // Was there a 4th face?
      if (facesRead == 12) {
        // Make the last vertex.
        Vertex v4(
            positions[i[9] - 1],
            normals[i[11] - 1],
            uvs[i[10] - 1]);

        // Add a whole triangle.
        mesh->AddTriangle(v1, v3, v4, Winding::CW);
      }
    }
  }

  obj.close();

  mesh->FinishBuilding();
  fileMap.insert_or_assign(filename, mesh);
  return mesh;
}

#pragma endregion

#pragma region OpenGL Mesh
#if defined(_OPENGL)

dg::OpenGLMesh::~OpenGLMesh() {
  if (VAO != 0) {
    glDeleteVertexArrays(1, &VAO);
    VAO = 0;
  }

  if (VBO != 0) {
    glDeleteBuffers(1, &VBO);
    VBO = 0;
  }

  if (EBO != 0) {
    glDeleteBuffers(1, &EBO);
    EBO = 0;
  }
}

void dg::OpenGLMesh::FinishBuilding() {
  assert(VAO == 0 && VBO == 0 && EBO == 0);

  const size_t positionSize = sizeof(Vertex::Data::position);
  const size_t normalSize = sizeof(Vertex::Data::normal);
  const size_t texCoordSize = sizeof(Vertex::Data::texCoord);
  const size_t tangentSize = sizeof(Vertex::Data::tangent);

  const size_t stride =
    (static_cast<bool>(attributes & Vertex::AttrFlag::POSITION)
      ? positionSize : 0) +
    (static_cast<bool>(attributes & Vertex::AttrFlag::NORMAL)
      ? normalSize : 0) +
    (static_cast<bool>(attributes & Vertex::AttrFlag::TEXCOORD)
      ? texCoordSize : 0) +
    (static_cast<bool>(attributes & Vertex::AttrFlag::TANGENT)
      ? tangentSize : 0);
  const int numVertices = vertexPositions.size();
  const size_t totalSize = numVertices * stride;

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
    indices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);

  size_t offset = 0;

  if (!!(attributes & Vertex::AttrFlag::POSITION)) {
    const size_t attribSize = positionSize;
    const size_t arraySize = numVertices * attribSize;
    glBufferSubData(GL_ARRAY_BUFFER, offset, arraySize, vertexPositions.data());
    glVertexAttribPointer(
        Vertex::AttrFlagToIndex(Vertex::AttrFlag::POSITION),
        attribSize / sizeof(float), GL_FLOAT, GL_FALSE, attribSize,
        (void*)offset);
    offset += arraySize;
  }

  if (!!(attributes & Vertex::AttrFlag::NORMAL)) {
    const size_t attribSize = normalSize;
    const size_t arraySize = numVertices * attribSize;
    glBufferSubData(GL_ARRAY_BUFFER, offset, arraySize, vertexNormals.data());
    glVertexAttribPointer(
        Vertex::AttrFlagToIndex(Vertex::AttrFlag::NORMAL),
        attribSize / sizeof(float), GL_FLOAT, GL_FALSE, attribSize,
        (void*)offset);
    offset += arraySize;
  }

  if (!!(attributes & Vertex::AttrFlag::TEXCOORD)) {
    const size_t attribSize = texCoordSize;
    const size_t arraySize = numVertices * attribSize;
    glBufferSubData(GL_ARRAY_BUFFER, offset, arraySize, vertexTexCoords.data());
    glVertexAttribPointer(
        Vertex::AttrFlagToIndex(Vertex::AttrFlag::TEXCOORD),
        attribSize / sizeof(float), GL_FLOAT, GL_FALSE, attribSize,
        (void*)offset);
    offset += arraySize;
  }

  if (!!(attributes & Vertex::AttrFlag::TANGENT)) {
    const size_t attribSize = tangentSize;
    const size_t arraySize = numVertices * attribSize;
    glBufferSubData(GL_ARRAY_BUFFER, offset, arraySize, vertexTangents.data());
    glVertexAttribPointer(
        Vertex::AttrFlagToIndex(Vertex::AttrFlag::TANGENT),
        attribSize / sizeof(float), GL_FLOAT, GL_FALSE, attribSize,
        (void*)offset);
    offset += arraySize;
  }

  vertexMap.clear();
}

void dg::OpenGLMesh::Draw() const {
  Mesh::Draw();

  glBindVertexArray(VAO);
  if (lastDrawnMesh != this) {
    for (int i = 0; i < Vertex::NumAttrs; i++) {
      if (static_cast<bool>(attributes & (Vertex::AttrFlag)(1 << i))) {
        glEnableVertexAttribArray(i);
      } else {
        glDisableVertexAttribArray(i);
      }
    }
    lastDrawnMesh = (Mesh*)this; // Although we're const, we'll allow this.
  }
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
}

bool dg::OpenGLMesh::IsDrawable() const {
  return (VAO != 0);
}

#endif
#pragma endregion
#pragma region DirectX Mesh
#if defined(_DIRECTX)

dg::DirectXMesh::~DirectXMesh() {
  if (vertexBuffer != nullptr) {
    vertexBuffer->Release();
    vertexBuffer = nullptr;
  }
  if (indexBuffer != nullptr) {
    indexBuffer->Release();
    indexBuffer = nullptr;
  }
}

void dg::DirectXMesh::FinishBuilding() {
  assert(vertexBuffer == nullptr);
  assert(indexBuffer == nullptr);

  // TODO: Create separate buffers for each attribute.

  std::vector<Vertex::Data> vertices(vertexPositions.size());
  int numVertices = (int)vertexPositions.size();
  for (int i = 0; i < numVertices; i++) {
    vertices[i] = GetVertex(i).data;
  }

  D3D11_BUFFER_DESC vbd;
  vbd.Usage = D3D11_USAGE_IMMUTABLE;
  vbd.ByteWidth = (unsigned int)(sizeof(Vertex::Data) * numVertices);
  vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vbd.CPUAccessFlags = 0;
  vbd.MiscFlags = 0;
  vbd.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA initialVertexData;
  initialVertexData.pSysMem = vertices.data();

  Graphics::Instance->device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);

  D3D11_BUFFER_DESC ibd;
  ibd.Usage = D3D11_USAGE_IMMUTABLE;
  ibd.ByteWidth = (unsigned int)(sizeof(int) * indices.size());
  ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  ibd.CPUAccessFlags = 0;
  ibd.MiscFlags = 0;
  ibd.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA initialIndexData;
  initialIndexData.pSysMem = indices.data();

  Graphics::Instance->device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer);

  vertexMap.clear();
}

void dg::DirectXMesh::Draw() const {
  assert(vertexBuffer != nullptr);
  assert(indexBuffer != nullptr);

  Mesh::Draw();

  UINT stride = sizeof(Vertex::Data);
  UINT offset = 0;
  Graphics::Instance->context->IASetVertexBuffers(
    0, 1, &vertexBuffer, &stride, &offset);
  Graphics::Instance->context->IASetIndexBuffer(
    indexBuffer, DXGI_FORMAT_R32_UINT, 0);

  Graphics::Instance->context->DrawIndexed((unsigned int)indices.size(), 0, 0);
}

bool dg::DirectXMesh::IsDrawable() const {
  return (vertexBuffer != NULL && indexBuffer != NULL);
}

#endif
#pragma endregion

