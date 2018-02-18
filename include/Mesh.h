//
//  Mesh.h
//

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <Shader.h>

namespace dg {

  struct Vertex {
    // Flags to keep trach of which attributes this vertex has,
    // since the vertex attribute vector fields aren't nulltable.
    enum class AttrFlag : uint32_t {
      NONE     = 0,
      POSITION = 1, // Attribute index = 0
      NORMAL   = 2, // Attribute index = 1
      TEXCOORD = 4, // Attribute index = 2
      TANGENT  = 8, // Attribute index = 3
    };
    static const int NumAttrs = 4;

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texCoord;
    AttrFlag attrFlags = AttrFlag::NONE;

    Vertex(glm::vec3 position)
      : position(position), attrFlags(AttrFlag::POSITION) {};
    Vertex(
        glm::vec3 position, glm::vec3 normal, glm::vec3 tangent,
        glm::vec2 texCoord)
      : position(position), attrFlags(
          AttrFlag::POSITION | AttrFlag::NORMAL) {};

    bool HasAllAttr(AttrFlag flags) {
      return (this->attrFlags & flags) == flags;
    }

    static int AttrFlagToIndex(AttrFlag flag) {
      for (int i = 0; i < NumAttrs; i++) {
        if (flag == (AttrFlag)(1 << i)) {
          return i;
        }
      }
      return -1;
    }

    using T = std::underlying_type_t<AttrFlag>;
    friend inline AttrFlag operator | (AttrFlag lhs, AttrFlag rhs) {
      return (AttrFlag)(static_cast<T>(lhs) | static_cast<T>(rhs));
    };
    friend inline AttrFlag operator |= (AttrFlag& lhs, AttrFlag rhs) {
      return lhs = lhs | rhs;
    }
    friend inline AttrFlag operator & (AttrFlag lhs, AttrFlag rhs) {
      return (AttrFlag)(static_cast<T>(lhs) & static_cast<T>(rhs));
    };
    friend inline bool operator == (AttrFlag lhs, AttrFlag rhs) {
      return static_cast<T>(lhs) == static_cast<T>(rhs);
    };
  };


  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL resources.
  class Mesh {

    public:

      enum class Winding { CW, CCW };

      static std::shared_ptr<Mesh> Cube;
      static std::shared_ptr<Mesh> MappedCube;
      static std::shared_ptr<Mesh> Quad;
      static std::shared_ptr<Mesh> Cylinder;
      static std::shared_ptr<Mesh> Sphere;

      static void CreatePrimitives();

      Mesh() = default;
      Mesh(Mesh& other) = delete;
      Mesh(Mesh&& other);
      ~Mesh();
      Mesh& operator=(Mesh& other) = delete;
      Mesh& operator=(Mesh&& other);
      friend void swap(Mesh& first, Mesh& second); // nothrow

      void Draw() const;

    private:

      // Ordered list of vertexes, broken down into lists of their individual
      // attributes. These lists will be the same size, and the same element
      // of each list belongs to the same vertex.
      std::vector<glm::vec3> vertexPositions;
      std::vector<glm::vec3> vertexNormals;
      std::vector<glm::vec3> vertexTangents;
      std::vector<glm::vec2> vertexTexCoords;
      Vertex::AttrFlag attributes = Vertex::AttrFlag::NONE;

      static std::unique_ptr<Mesh> CreateCube();
      static std::unique_ptr<Mesh> CreateMappedCube();
      static std::unique_ptr<Mesh> CreateQuad();
      static std::unique_ptr<Mesh> CreateCylinder(
          int radialDivisions, int heightDivisions);
      static std::unique_ptr<Mesh> CreateSphere(int subdivisions);

      static Mesh *lastDrawnMesh;

      GLenum drawMode = 0;
      GLsizei drawCount = 0;
      GLuint VAO = 0;
      GLuint VBO = 0;

  }; // class Mesh

} // namespace dg
