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

enum {
  ATTR_POSITION  = 0,
  ATTR_NORMAL    = 1,
  ATTR_TEX_COORD = 2,
};

static float cubeVertices[] = {
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

static float quadVertices[] = {
  // positions          // texture   // normals
  //                    // coords    //

  -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
   0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
   0.5f,  0.5f,  0.0f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
   0.5f,  0.5f,  0.0f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
  -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
  -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
};

std::shared_ptr<dg::Mesh> dg::Mesh::Cube = nullptr;
std::shared_ptr<dg::Mesh> dg::Mesh::Quad = nullptr;

void dg::Mesh::CreatePrimitives() {
  assert(Mesh::Cube == nullptr);
  dg::Mesh::Cube = std::shared_ptr<Mesh>(CreateCube());

  assert(Mesh::Quad == nullptr);
  dg::Mesh::Quad = std::shared_ptr<Mesh>(CreateQuad());
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
}

void dg::Mesh::Use() const {
  glBindVertexArray(VAO);
  glEnableVertexAttribArray(ATTR_POSITION);
  glEnableVertexAttribArray(ATTR_NORMAL);
  glEnableVertexAttribArray(ATTR_TEX_COORD);
}

void dg::Mesh::Draw() const {
  glBindVertexArray(VAO);
  glDrawArrays(drawMode, 0, drawCount);
}

void dg::Mesh::FinishUsing() const {
  glDisableVertexAttribArray(ATTR_POSITION);
  glDisableVertexAttribArray(ATTR_NORMAL);
  glDisableVertexAttribArray(ATTR_TEX_COORD);
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

  glVertexAttribPointer(
      ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
      (void*)(3 * sizeof(float)));

  glVertexAttribPointer(
      ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
      (void*)(5 * sizeof(float)));

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

  glVertexAttribPointer(
      ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

  glVertexAttribPointer(
      ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
      (void*)(3 * sizeof(float)));

  glVertexAttribPointer(
      ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
      (void*)(5 * sizeof(float)));

  mesh->drawMode = GL_TRIANGLES;
  mesh->drawCount = 6;

  return mesh;
}

