//
//  Mesh.cpp
//

#include <glad/glad.h>
#include <Mesh.h>

#include <cassert>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

static const char *positionAttributeName = "in_Position";
static const char *texCoordAttributeName = "in_TexCoord";

static const float cubeVertices[] = {
  // positions          // texture   // normals
  //                    // coords    //

  // Back face
   0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
  -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
   0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
   0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
  -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,

  // Front face
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

  // Left face
  -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
  -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,

  // Right face
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
   0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
   0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,

   // Bottom face
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,

  // Top face
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
};

static const float Q = 1.f / 4.f; // quater
static const float T = 1.f / 3.f; // third
static const float E = 1.f / (2048.f / 4.f * 3.f);    // epsilon to hide seams

static const glm::vec2 BACK_TL   (Q*3+E, T*2-E);
static const glm::vec2 BACK_TR   (Q*4-E, T*2-E);
static const glm::vec2 BACK_BL   (Q*3+E, T*1+E);
static const glm::vec2 BACK_BR   (Q*4-E, T*1+E);

static const glm::vec2 FRONT_TL  (Q*1+E, T*2-E);
static const glm::vec2 FRONT_TR  (Q*2-E, T*2-E);
static const glm::vec2 FRONT_BL  (Q*1+E, T*1+E);
static const glm::vec2 FRONT_BR  (Q*2-E, T*1+E);

static const glm::vec2 LEFT_TL   (Q*0+E, T*2-E);
static const glm::vec2 LEFT_TR   (Q*1-E, T*2-E);
static const glm::vec2 LEFT_BL   (Q*0+E, T*1+E);
static const glm::vec2 LEFT_BR   (Q*1-E, T*1+E);

static const glm::vec2 RIGHT_TL  (Q*2+E, T*2-E);
static const glm::vec2 RIGHT_TR  (Q*3-E, T*2-E);
static const glm::vec2 RIGHT_BL  (Q*2+E, T*1+E);
static const glm::vec2 RIGHT_BR  (Q*3-E, T*1+E);

static const glm::vec2 BOTTOM_TL (Q*1+E, T*1-E);
static const glm::vec2 BOTTOM_TR (Q*2-E, T*1-E);
static const glm::vec2 BOTTOM_BL (Q*1+E, T*0+E);
static const glm::vec2 BOTTOM_BR (Q*2-E, T*0+E);

static const glm::vec2 TOP_TL    (Q*1+E, T*3-E);
static const glm::vec2 TOP_TR    (Q*2-E, T*3-E);
static const glm::vec2 TOP_BL    (Q*1+E, T*2+E);
static const glm::vec2 TOP_BR    (Q*2-E, T*2+E);

static const float mappedCubeVertices[] = {
  // positions          // texture coords          // normals

  // Back face
   0.5f, -0.5f, -0.5f,  BACK_BL.x,   BACK_BL.y,    0.0f,  0.0f, -1.0f, // BL
  -0.5f, -0.5f, -0.5f,  BACK_BR.x,   BACK_BR.y,    0.0f,  0.0f, -1.0f, // BR
   0.5f,  0.5f, -0.5f,  BACK_TL.x,   BACK_TL.y,    0.0f,  0.0f, -1.0f, // TL
  -0.5f,  0.5f, -0.5f,  BACK_TR.x,   BACK_TR.y,    0.0f,  0.0f, -1.0f, // TR
   0.5f,  0.5f, -0.5f,  BACK_TL.x,   BACK_TL.y,    0.0f,  0.0f, -1.0f, // TL
  -0.5f, -0.5f, -0.5f,  BACK_BR.x,   BACK_BR.y,    0.0f,  0.0f, -1.0f, // BR

  // Front face
  -0.5f, -0.5f,  0.5f,  FRONT_BL.x,  FRONT_BL.y,   0.0f,  0.0f,  1.0f, // BL
   0.5f, -0.5f,  0.5f,  FRONT_BR.x,  FRONT_BR.y,   0.0f,  0.0f,  1.0f, // BR
   0.5f,  0.5f,  0.5f,  FRONT_TR.x,  FRONT_TR.y,   0.0f,  0.0f,  1.0f, // TR
   0.5f,  0.5f,  0.5f,  FRONT_TR.x,  FRONT_TR.y,   0.0f,  0.0f,  1.0f, // TR
  -0.5f,  0.5f,  0.5f,  FRONT_TL.x,  FRONT_TL.y,   0.0f,  0.0f,  1.0f, // TL
  -0.5f, -0.5f,  0.5f,  FRONT_BL.x,  FRONT_BL.y,   0.0f,  0.0f,  1.0f, // BL

  // Left face
  -0.5f,  0.5f,  0.5f,  LEFT_TR.x,   LEFT_TR.y,   -1.0f,  0.0f,  0.0f, // TR
  -0.5f,  0.5f, -0.5f,  LEFT_TL.x,   LEFT_TL.y,   -1.0f,  0.0f,  0.0f, // TL
  -0.5f, -0.5f, -0.5f,  LEFT_BL.x,   LEFT_BL.y,   -1.0f,  0.0f,  0.0f, // BL
  -0.5f, -0.5f, -0.5f,  LEFT_BL.x,   LEFT_BL.y,   -1.0f,  0.0f,  0.0f, // BL
  -0.5f, -0.5f,  0.5f,  LEFT_BR.x,   LEFT_BR.y,   -1.0f,  0.0f,  0.0f, // BR
  -0.5f,  0.5f,  0.5f,  LEFT_TR.x,   LEFT_TR.y,   -1.0f,  0.0f,  0.0f, // TR

  // Right face
   0.5f,  0.5f, -0.5f,  RIGHT_TR.x,  RIGHT_TR.y,   1.0f,  0.0f,  0.0f, // TR
   0.5f,  0.5f,  0.5f,  RIGHT_TL.x,  RIGHT_TL.y,   1.0f,  0.0f,  0.0f, // TL
   0.5f, -0.5f, -0.5f,  RIGHT_BR.x,  RIGHT_BR.y,   1.0f,  0.0f,  0.0f, // BR
   0.5f, -0.5f,  0.5f,  RIGHT_BL.x,  RIGHT_BL.y,   1.0f,  0.0f,  0.0f, // BL
   0.5f, -0.5f, -0.5f,  RIGHT_BR.x,  RIGHT_BR.y,   1.0f,  0.0f,  0.0f, // BR
   0.5f,  0.5f,  0.5f,  RIGHT_TL.x,  RIGHT_TL.y,   1.0f,  0.0f,  0.0f, // TL

   // Bottom face
  -0.5f, -0.5f, -0.5f,  BOTTOM_BL.x, BOTTOM_BL.y,  0.0f, -1.0f,  0.0f, // BL
   0.5f, -0.5f, -0.5f,  BOTTOM_BR.x, BOTTOM_BR.y,  0.0f, -1.0f,  0.0f, // BR
   0.5f, -0.5f,  0.5f,  BOTTOM_TR.x, BOTTOM_TR.y,  0.0f, -1.0f,  0.0f, // TR
   0.5f, -0.5f,  0.5f,  BOTTOM_TR.x, BOTTOM_TR.y,  0.0f, -1.0f,  0.0f, // TR
  -0.5f, -0.5f,  0.5f,  BOTTOM_TL.x, BOTTOM_TL.y,  0.0f, -1.0f,  0.0f, // TL
  -0.5f, -0.5f, -0.5f,  BOTTOM_BL.x, BOTTOM_BL.y,  0.0f, -1.0f,  0.0f, // BL

  // Top face
   0.5f,  0.5f, -0.5f,  TOP_TR.x,   TOP_TR.y,     0.0f,  1.0f,  0.0f, // TR
  -0.5f,  0.5f, -0.5f,  TOP_TL.x,   TOP_TL.y,     0.0f,  1.0f,  0.0f, // TL
   0.5f,  0.5f,  0.5f,  TOP_BR.x,   TOP_BR.y,     0.0f,  1.0f,  0.0f, // BR
  -0.5f,  0.5f,  0.5f,  TOP_BL.x,   TOP_BL.y,     0.0f,  1.0f,  0.0f, // BL
   0.5f,  0.5f,  0.5f,  TOP_BR.x,   TOP_BR.y,     0.0f,  1.0f,  0.0f, // BR
  -0.5f,  0.5f, -0.5f,  TOP_TL.x,   TOP_TL.y,     0.0f,  1.0f,  0.0f, // TL
};

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

dg::Mesh::Mesh() {
  for (int attr = 0; attr < ATTR_MAX; attr++) {
    useAttribute[attr] = false;
  }
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
  swap(first.useAttribute, second.useAttribute);
}

void dg::Mesh::Use() const {
  glBindVertexArray(VAO);
  for (int attr = 0; attr < ATTR_MAX; attr++) {
    if (useAttribute[attr]) {
      glEnableVertexAttribArray(attr);
    }
  }
}

void dg::Mesh::Draw() const {
  glBindVertexArray(VAO);
  glDrawArrays(drawMode, 0, drawCount);
}

void dg::Mesh::FinishUsing() const {
  for (int attr = 0; attr < ATTR_MAX; attr++) {
    if (useAttribute[attr]) {
      glDisableVertexAttribArray(attr);
    }
  }
}

std::unique_ptr<dg::Mesh> dg::Mesh::CreateCube() {
  std::unique_ptr<Mesh> mesh = std::unique_ptr<Mesh>(new Mesh());

  glGenVertexArrays(1, &mesh->VAO);
  glBindVertexArray(mesh->VAO);

  glGenBuffers(1, &mesh->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
  glBufferData(
      GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(
      ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  mesh->useAttribute[ATTR_POSITION] = true;

  glVertexAttribPointer(
      ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
      (void*)(3 * sizeof(float)));
  mesh->useAttribute[ATTR_TEX_COORD] = true;

  glVertexAttribPointer(
      ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
      (void*)(5 * sizeof(float)));
  mesh->useAttribute[ATTR_NORMAL] = true;

  mesh->drawMode = GL_TRIANGLES;
  mesh->drawCount = 36;

  return mesh;
}

std::unique_ptr<dg::Mesh> dg::Mesh::CreateMappedCube() {
  std::unique_ptr<Mesh> mesh = std::unique_ptr<Mesh>(new Mesh());

  glGenVertexArrays(1, &mesh->VAO);
  glBindVertexArray(mesh->VAO);

  glGenBuffers(1, &mesh->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
  glBufferData(
      GL_ARRAY_BUFFER, sizeof(mappedCubeVertices), mappedCubeVertices,
      GL_STATIC_DRAW);

  glVertexAttribPointer(
      ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  mesh->useAttribute[ATTR_POSITION] = true;

  glVertexAttribPointer(
      ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
      (void*)(3 * sizeof(float)));
  mesh->useAttribute[ATTR_TEX_COORD] = true;

  glVertexAttribPointer(
      ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
      (void*)(5 * sizeof(float)));
  mesh->useAttribute[ATTR_NORMAL] = true;

  mesh->drawMode = GL_TRIANGLES;
  mesh->drawCount = 36;

  return mesh;
}

std::unique_ptr<dg::Mesh> dg::Mesh::CreateQuad() {
  std::unique_ptr<Mesh> mesh = std::unique_ptr<Mesh>(new Mesh());

  glGenVertexArrays(1, &mesh->VAO);
  glBindVertexArray(mesh->VAO);

  glGenBuffers(1, &mesh->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
  glBufferData(
      GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

  GLsizei stride = 0;
  stride += 3 * sizeof(float); // position
  stride += 2 * sizeof(float); // texture coords
  stride += 3 * sizeof(float); // normals
  stride += 3 * sizeof(float); // tangents

  long offset = 0;

  glVertexAttribPointer(
      ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
  offset += 3 * sizeof(float);
  mesh->useAttribute[ATTR_POSITION] = true;

  glVertexAttribPointer(
      ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
  offset += 2 * sizeof(float);
  mesh->useAttribute[ATTR_TEX_COORD] = true;

  glVertexAttribPointer(
      ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
  offset += 3 * sizeof(float);
  mesh->useAttribute[ATTR_NORMAL] = true;

  glVertexAttribPointer(
      ATTR_TANGENT, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
  offset += 3 * sizeof(float);
  mesh->useAttribute[ATTR_TANGENT] = true;

  mesh->drawMode = GL_TRIANGLES;
  mesh->drawCount = 6;

  return mesh;
}

std::unique_ptr<dg::Mesh> dg::Mesh::CreateCylinder(
    int radialDivisions, int heightDivisions) {
  if (radialDivisions < 3) {
    radialDivisions = 3;
  }

  if (heightDivisions < 1) {
    heightDivisions = 1;
  }

  int numTriangles = radialDivisions * (1 + 1 + (2 * heightDivisions));
  int positionSize = 3;
  int normalSize = 3;
  int texCoordSize = 2;
  int vertexStride = positionSize + normalSize + texCoordSize;
  std::vector<float> buffer(3 * numTriangles * vertexStride);

  int positionOffset = 0;
  int normalOffset = positionSize;
  int texCoordOffset = normalOffset + normalSize;

  float halfHeight = 0.5f;
  float radInterval = glm::radians(360.f) / (float)radialDivisions;
  float radius = 0.5f;

  int nextVertOffset = 0;
  auto AddTriangle = [&](
      glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
      glm::vec3 n1, glm::vec3 n2, glm::vec3 n3,
      glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3) {
    buffer[nextVertOffset + positionOffset + 0] = v1.x;
    buffer[nextVertOffset + positionOffset + 1] = v1.y;
    buffer[nextVertOffset + positionOffset + 2] = v1.z;
    buffer[nextVertOffset + normalOffset + 0] = n1.x;
    buffer[nextVertOffset + normalOffset + 1] = n1.y;
    buffer[nextVertOffset + normalOffset + 2] = n1.z;
    buffer[nextVertOffset + texCoordOffset + 0] = uv1.x;
    buffer[nextVertOffset + texCoordOffset + 1] = uv1.y;
    nextVertOffset += vertexStride;

    buffer[nextVertOffset + positionOffset + 0] = v2.x;
    buffer[nextVertOffset + positionOffset + 1] = v2.y;
    buffer[nextVertOffset + positionOffset + 2] = v2.z;
    buffer[nextVertOffset + normalOffset + 0] = n2.x;
    buffer[nextVertOffset + normalOffset + 1] = n2.y;
    buffer[nextVertOffset + normalOffset + 2] = n2.z;
    buffer[nextVertOffset + texCoordOffset + 0] = uv2.x;
    buffer[nextVertOffset + texCoordOffset + 1] = uv2.y;
    nextVertOffset += vertexStride;

    buffer[nextVertOffset + positionOffset + 0] = v3.x;
    buffer[nextVertOffset + positionOffset + 1] = v3.y;
    buffer[nextVertOffset + positionOffset + 2] = v3.z;
    buffer[nextVertOffset + normalOffset + 0] = n3.x;
    buffer[nextVertOffset + normalOffset + 1] = n3.y;
    buffer[nextVertOffset + normalOffset + 2] = n3.z;
    buffer[nextVertOffset + texCoordOffset + 0] = uv3.x;
    buffer[nextVertOffset + texCoordOffset + 1] = uv3.y;
    nextVertOffset += vertexStride;
  };

  for (int i = 0; i < radialDivisions; i++) {
    glm::vec3 leftNormal = glm::quat(
        glm::vec3(0, radInterval * i, 0)) * -FORWARD;
    glm::vec3 rightNormal = glm::quat(
        glm::vec3(0, radInterval * (i + 1), 0)) * -FORWARD;

    glm::vec3 topLeft = (leftNormal * radius) + glm::vec3(0, halfHeight, 0);
    glm::vec3 topRight = (rightNormal * radius) + glm::vec3(0, halfHeight, 0);
    glm::vec3 bottomLeft = (leftNormal * radius) - glm::vec3(0, halfHeight, 0);
    glm::vec3 bottomRight =
      (rightNormal * radius) - glm::vec3(0, halfHeight, 0);

    glm::vec3 topCenter = glm::vec3(0, halfHeight, 0);
    glm::vec3 bottomCenter = glm::vec3(0, -halfHeight, 0);

    glm::vec2 uvTopCenter = glm::vec2(1.f / 6);
    glm::vec2 uvTopExtents = glm::vec2(1.f / 3) * 0.5f;
    glm::vec2 uvBottomCenter = uvTopCenter;
    uvBottomCenter.y = 1 - uvBottomCenter.y;
    glm::vec2 uvBottomExtents = uvTopExtents;

    // Add top triangle.
    AddTriangle(
        topCenter, topLeft, topRight,
        UP, UP, UP,
        uvTopCenter,
        uvTopCenter + uvTopExtents * glm::vec2(leftNormal.x, leftNormal.z),
        uvTopCenter + uvTopExtents * glm::vec2(rightNormal.x, rightNormal.z));

    // Add bottom triangle.
    AddTriangle(
        bottomCenter, bottomRight, bottomLeft,
        -UP, -UP, -UP,
        uvBottomCenter,
        uvBottomCenter + uvBottomExtents * glm::vec2(
          rightNormal.x, rightNormal.z),
        uvBottomCenter + uvBottomExtents * glm::vec2(
            leftNormal.x, leftNormal.z));

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

      AddTriangle(
          quadBottomLeft, quadTopRight, quadTopLeft,
          leftNormal, rightNormal, leftNormal,
          uvBottomLeft, uvTopRight, uvTopLeft);
      AddTriangle(
          quadBottomRight, quadTopRight, quadBottomLeft,
          rightNormal, rightNormal, leftNormal,
          uvBottomRight, uvTopRight, uvBottomLeft);
    }
  }

  std::unique_ptr<Mesh> mesh = std::unique_ptr<Mesh>(new Mesh());

  glGenVertexArrays(1, &mesh->VAO);
  glBindVertexArray(mesh->VAO);

  glGenBuffers(1, &mesh->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
  glBufferData(
      GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(),
      GL_STATIC_DRAW);

  glVertexAttribPointer(
      ATTR_POSITION, positionSize, GL_FLOAT, GL_FALSE,
      vertexStride * sizeof(float), (void*)(positionOffset * sizeof(float)));
  mesh->useAttribute[ATTR_POSITION] = true;

  glVertexAttribPointer(
      ATTR_NORMAL, normalSize, GL_FLOAT, GL_FALSE,
      vertexStride * sizeof(float), (void*)(normalOffset * sizeof(float)));
  mesh->useAttribute[ATTR_NORMAL] = true;

  glVertexAttribPointer(
      ATTR_TEX_COORD, texCoordSize, GL_FLOAT, GL_FALSE,
      vertexStride * sizeof(float), (void*)(texCoordOffset * sizeof(float)));
  mesh->useAttribute[ATTR_TEX_COORD] = true;

  mesh->drawMode = GL_TRIANGLES;
  mesh->drawCount = numTriangles * 3;

  return mesh;
}

std::unique_ptr<dg::Mesh> dg::Mesh::CreateSphere(int subdivisions) {
  if (subdivisions < 3) {
    subdivisions = 3;
  }

  int numTriangles = subdivisions * (1 + 1 + (2 * subdivisions));
  int positionSize = 3;
  int normalSize = 3;
  int texCoordSize = 2;
  int vertexStride = positionSize + normalSize + texCoordSize;
  std::vector<float> buffer(3 * numTriangles * vertexStride);

  int positionOffset = 0;
  int normalOffset = positionSize;
  int texCoordOffset = normalOffset + normalSize;

  float radInterval = glm::radians(360.f) / (float)subdivisions;
  float radius = 0.5f;

  int nextVertOffset = 0;
  auto AddTriangle = [&](
      glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
      glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3) {

    // All vertices on a sphere will be on the surface, thus their
    // normal will also be their normalized position.
    glm::vec3 n1 = glm::normalize(v1);
    glm::vec3 n2 = glm::normalize(v2);
    glm::vec3 n3 = glm::normalize(v3);

    buffer[nextVertOffset + positionOffset + 0] = v1.x;
    buffer[nextVertOffset + positionOffset + 1] = v1.y;
    buffer[nextVertOffset + positionOffset + 2] = v1.z;
    buffer[nextVertOffset + normalOffset + 0] = n1.x;
    buffer[nextVertOffset + normalOffset + 1] = n1.y;
    buffer[nextVertOffset + normalOffset + 2] = n1.z;
    buffer[nextVertOffset + texCoordOffset + 0] = uv1.x;
    buffer[nextVertOffset + texCoordOffset + 1] = uv1.y;
    nextVertOffset += vertexStride;

    buffer[nextVertOffset + positionOffset + 0] = v2.x;
    buffer[nextVertOffset + positionOffset + 1] = v2.y;
    buffer[nextVertOffset + positionOffset + 2] = v2.z;
    buffer[nextVertOffset + normalOffset + 0] = n2.x;
    buffer[nextVertOffset + normalOffset + 1] = n2.y;
    buffer[nextVertOffset + normalOffset + 2] = n2.z;
    buffer[nextVertOffset + texCoordOffset + 0] = uv2.x;
    buffer[nextVertOffset + texCoordOffset + 1] = uv2.y;
    nextVertOffset += vertexStride;

    buffer[nextVertOffset + positionOffset + 0] = v3.x;
    buffer[nextVertOffset + positionOffset + 1] = v3.y;
    buffer[nextVertOffset + positionOffset + 2] = v3.z;
    buffer[nextVertOffset + normalOffset + 0] = n3.x;
    buffer[nextVertOffset + normalOffset + 1] = n3.y;
    buffer[nextVertOffset + normalOffset + 2] = n3.z;
    buffer[nextVertOffset + texCoordOffset + 0] = uv3.x;
    buffer[nextVertOffset + texCoordOffset + 1] = uv3.y;
    nextVertOffset += vertexStride;
  };

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

      float uvHeightInterval = 1.f / subdivisions;
      float uvBottomHeight = uvHeightInterval * j;
      float uvTopHeight = uvHeightInterval * (j + 1);
      glm::vec2 uvBottomLeft((float)i / subdivisions, uvBottomHeight);
      glm::vec2 uvBottomRight((float)(i + 1) / subdivisions, uvBottomHeight);
      glm::vec2 uvTopLeft((float)i / subdivisions, uvTopHeight);
      glm::vec2 uvTopRight((float)(i + 1) / subdivisions, uvTopHeight);

      AddTriangle(
          bottomLeft, topRight, topLeft,
          uvBottomLeft, uvTopRight, uvTopLeft);
      AddTriangle(
          bottomRight, topRight, bottomLeft,
          uvBottomRight, uvTopRight, uvBottomLeft);
    }
  }

  std::unique_ptr<Mesh> mesh = std::unique_ptr<Mesh>(new Mesh());

  glGenVertexArrays(1, &mesh->VAO);
  glBindVertexArray(mesh->VAO);

  glGenBuffers(1, &mesh->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
  glBufferData(
      GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(),
      GL_STATIC_DRAW);

  glVertexAttribPointer(
      ATTR_POSITION, positionSize, GL_FLOAT, GL_FALSE,
      vertexStride * sizeof(float), (void*)(positionOffset * sizeof(float)));
  mesh->useAttribute[ATTR_POSITION] = true;

  glVertexAttribPointer(
      ATTR_NORMAL, normalSize, GL_FLOAT, GL_FALSE,
      vertexStride * sizeof(float), (void*)(normalOffset * sizeof(float)));
  mesh->useAttribute[ATTR_NORMAL] = true;

  glVertexAttribPointer(
      ATTR_TEX_COORD, texCoordSize, GL_FLOAT, GL_FALSE,
      vertexStride * sizeof(float), (void*)(texCoordOffset * sizeof(float)));
  mesh->useAttribute[ATTR_TEX_COORD] = true;

  mesh->drawMode = GL_TRIANGLES;
  mesh->drawCount = numTriangles * 3;

  return mesh;
}

