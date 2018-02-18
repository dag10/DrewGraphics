//
//  Mesh.cpp
//

#include <glad/glad.h>
#include <Mesh.h>

#include <cassert>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

dg::Mesh *dg::Mesh::lastDrawnMesh = nullptr;


static const float quadVertices[] = {
  // positions         // texture // normals // tangents
  //                   // coords  //

  -0.5f, -0.5f,  0.0f, 0, 0,      0, 0, 1,    1, 0, 0,
   0.5f, -0.5f,  0.0f, 1, 0,      0, 0, 1,    1, 0, 0,
   0.5f,  0.5f,  0.0f, 1, 1,      0, 0, 1,    1, 0, 0,
   0.5f,  0.5f,  0.0f, 1, 1,      0, 0, 1,    1, 0, 0,
  -0.5f,  0.5f,  0.0f, 0, 1,      0, 0, 1,    1, 0, 0,
  -0.5f, -0.5f,  0.0f, 0, 0,      0, 0, 1,    1, 0, 0,
};

std::shared_ptr<dg::Mesh> dg::Mesh::Cube = nullptr;
std::shared_ptr<dg::Mesh> dg::Mesh::MappedCube = nullptr;
std::shared_ptr<dg::Mesh> dg::Mesh::Quad = nullptr;
std::shared_ptr<dg::Mesh> dg::Mesh::Cylinder = nullptr;
std::shared_ptr<dg::Mesh> dg::Mesh::Sphere = nullptr;

void dg::Mesh::CreatePrimitives() {
  assert(Mesh::Cube == nullptr);
  dg::Mesh::Cube = std::shared_ptr<Mesh>(CreateCube());

  assert(Mesh::MappedCube == nullptr);
  dg::Mesh::MappedCube = std::shared_ptr<Mesh>(CreateMappedCube());

  assert(Mesh::Quad == nullptr);
  dg::Mesh::Quad = std::shared_ptr<Mesh>(CreateQuad());

  assert(Mesh::Cylinder == nullptr);
  dg::Mesh::Cylinder = std::shared_ptr<Mesh>(CreateCylinder(64, 1));

  assert(Mesh::Sphere == nullptr);
  dg::Mesh::Sphere = std::shared_ptr<Mesh>(CreateSphere(32));
}

dg::Mesh::Mesh(dg::Mesh&& other) {
  *this = std::move(other);
}

dg::Mesh::~Mesh() {
  if (VAO != 0) {
    glDeleteVertexArrays(1, &VAO);
    VAO = 0;
  }

  if (VBO != 0) {
    glDeleteBuffers(1, &VBO);
    VBO = 0;
  }
}

dg::Mesh& dg::Mesh::operator=(dg::Mesh&& other) {
  swap(*this, other);
  return *this;
}

void dg::swap(Mesh& first, Mesh& second) {
  using std::swap;
  swap(first.drawMode, second.drawMode);
  swap(first.drawCount, second.drawCount);
  swap(first.VAO, second.VAO);
  swap(first.VBO, second.VBO);
  swap(first.vertexPositions, second.vertexPositions);
  swap(first.vertexNormals, second.vertexNormals);
  swap(first.vertexTangents, second.vertexTangents);
  swap(first.vertexTexCoords, second.vertexTexCoords);
  swap(first.attributes, second.attributes);
}

void dg::Mesh::AddQuad(
    Vertex v1, Vertex v2, Vertex v3, Vertex v4, Winding winding) {
  AddTriangle(v1, v2, v3, winding);
  AddTriangle(v1, v3, v4, winding);
}

void dg::Mesh::AddTriangle(Vertex v1, Vertex v2, Vertex v3, Winding winding) {
  if (attributes == Vertex::AttrFlag::NONE) {
    attributes = v1.attributes;
  }

  if (v1.attributes != v2.attributes || v1.attributes != v3.attributes) {
    throw std::runtime_error(
        "Attempted to add a triangle to a mesh when it has mismatched "
        "attributes.");
  }

  if (v1.attributes != attributes) {
    throw std::runtime_error(
        "Attempted to add a triangle to a mesh with noncompatible attributes.");
  }

  if (winding == Winding::CCW) {
    std::swap(v1, v2);
  }

  // TODO: Use a vertex->index map to find identical existing vertexes
  //       and reuse their index.

  if (v1.HasAllAttr(Vertex::AttrFlag::POSITION)) {
    vertexPositions.push_back(v1.position);
    vertexPositions.push_back(v2.position);
    vertexPositions.push_back(v3.position);
  }

  if (v1.HasAllAttr(Vertex::AttrFlag::NORMAL)) {
    vertexNormals.push_back(v1.normal);
    vertexNormals.push_back(v2.normal);
    vertexNormals.push_back(v3.normal);
  }

  if (v1.HasAllAttr(Vertex::AttrFlag::TEXCOORD)) {
    vertexTexCoords.push_back(v1.texCoord);
    vertexTexCoords.push_back(v2.texCoord);
    vertexTexCoords.push_back(v3.texCoord);
  }

  if (v1.HasAllAttr(Vertex::AttrFlag::TANGENT)) {
    vertexTangents.push_back(v1.tangent);
    vertexTangents.push_back(v2.tangent);
    vertexTangents.push_back(v3.tangent);
  }
}

void dg::Mesh::FinishBuilding() {
  assert(VAO == 0 && VBO == 0);

  const size_t positionSize = sizeof(Vertex::position);
  const size_t normalSize = sizeof(Vertex::normal);
  const size_t texCoordSize = sizeof(Vertex::texCoord);
  const size_t tangentSize = sizeof(Vertex::tangent);

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

  drawMode = GL_TRIANGLES;
  drawCount = numVertices;
}

void dg::Mesh::Draw() const {
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
  glDrawArrays(drawMode, 0, drawCount);
}

std::unique_ptr<dg::Mesh> dg::Mesh::CreateCube() {
  auto mesh = std::unique_ptr<Mesh>(new Mesh());
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

std::unique_ptr<dg::Mesh> dg::Mesh::CreateMappedCube() {
  auto mesh = std::unique_ptr<Mesh>(new Mesh());

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

std::unique_ptr<dg::Mesh> dg::Mesh::CreateQuad() {
  auto mesh = std::unique_ptr<Mesh>(new Mesh());
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

std::unique_ptr<dg::Mesh> dg::Mesh::CreateCylinder(
    int radialDivisions, int heightDivisions) {
  auto mesh = std::unique_ptr<Mesh>(new Mesh());

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

std::unique_ptr<dg::Mesh> dg::Mesh::CreateSphere(int subdivisions) {
  auto mesh = std::unique_ptr<Mesh>(new Mesh());

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

