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
    glm::vec2 texCoord;
    glm::vec3 tangent;
    AttrFlag attributes = AttrFlag::NONE;

    Vertex(glm::vec3 position)
      : position(position), attributes(AttrFlag::POSITION) {};
    Vertex(
        glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord)
      : position(position), normal(normal), texCoord(texCoord),
      attributes(AttrFlag::POSITION | AttrFlag::NORMAL | AttrFlag::TEXCOORD) {};
    Vertex(
        glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord,
        glm::vec3 tangent)
      : position(position), normal(normal), texCoord(texCoord),
      tangent(tangent), attributes(
          AttrFlag::POSITION | AttrFlag::NORMAL |
          AttrFlag::TEXCOORD | AttrFlag::TANGENT) {};

    bool HasAllAttr(AttrFlag flags) {
      return (this->attributes & flags) == flags;
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
    friend inline bool operator ! (AttrFlag flag) {
      return (static_cast<T>(flag) == 0);
    }
  };


  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL resources.
  class Mesh {

    public:

      enum class Winding { CW, CCW };

      struct Triangle {
        Vertex vertices[3];
        Winding winding;
      };

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

      void AddQuad(
          Vertex v1, Vertex v2, Vertex v3, Vertex v4, Winding winding);
      void AddTriangle(Vertex v1, Vertex v2, Vertex v3, Winding winding);
      void FinishBuilding();

      void Draw() const;

    private:

      // Ordered list of vertexes, broken down into lists of their individual
      // attributes. These lists will be the same size, and the same element
      // of each list belongs to the same vertex.
      std::vector<glm::vec3> vertexPositions;
      std::vector<glm::vec3> vertexNormals;
      std::vector<glm::vec2> vertexTexCoords;
      std::vector<glm::vec3> vertexTangents;
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
