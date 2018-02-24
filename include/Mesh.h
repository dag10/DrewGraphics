//
//  Mesh.h
//

#pragma once

#if defined(_OPENGL)
#include <glad/glad.h>
#elif defined(_DIRECTX)
#include <d3d11.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

namespace dg {

  class OpenGLMesh;
  class DirectXMesh;

  struct Vertex {
    typedef std::size_t hash_type;

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

    struct Data {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 texCoord;
      glm::vec3 tangent;
    };

    Data data;
    AttrFlag attributes = AttrFlag::NONE;

    Vertex(glm::vec3 position);
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord);
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord,
           glm::vec3 tangent);

    bool HasAllAttr(AttrFlag flags) const {
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

  // Copy is disabled. This prevents us from leaking or redeleting
  // OpenGL/DirectX resources.
  class Mesh {

    public:

#if defined(_OPENGL)
      typedef OpenGLMesh mesh_class;
#elif defined(_DIRECTX)
      typedef DirectXMesh mesh_class;
#endif

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

      static std::shared_ptr<Mesh> Create();
      static std::shared_ptr<Mesh> LoadOBJ(const char *filename);

      virtual ~Mesh() = default;

      Mesh(Mesh& other) = delete;
      Mesh& operator=(Mesh& other) = delete;

      void AddQuad(
          Vertex v1, Vertex v2, Vertex v3, Vertex v4, Winding winding);
      void AddTriangle(Vertex v1, Vertex v2, Vertex v3, Winding winding);
      virtual void FinishBuilding() = 0;

      const Vertex GetVertex(int i) const;

      virtual void Draw() const = 0;

    protected:

      Mesh() = default;

      // Ordered list of vertexes, broken down into lists of their individual
      // attributes. These lists will be the same size, and the same element
      // of each list belongs to the same vertex.
      std::vector<glm::vec3> vertexPositions;
      std::vector<glm::vec3> vertexNormals;
      std::vector<glm::vec2> vertexTexCoords;
      std::vector<glm::vec3> vertexTangents;
      std::vector<unsigned int> indices;

      // Bitmask of which attributes this mesh's vertices have.
      // If no vertices added yet, value is NONE.
      Vertex::AttrFlag attributes = Vertex::AttrFlag::NONE;

      // Map of hash of vertex to index of vertex already in vertex list.
      std::unordered_map<Vertex::hash_type, unsigned int> vertexMap;

      static std::shared_ptr<Mesh> CreateCube();
      static std::shared_ptr<Mesh> CreateMappedCube();
      static std::shared_ptr<Mesh> CreateQuad();
      static std::shared_ptr<Mesh> CreateCylinder(
          int radialDivisions, int heightDivisions);
      static std::shared_ptr<Mesh> CreateSphere(int subdivisions);

      static Mesh *lastDrawnMesh;
      static std::unordered_map<std::string, std::weak_ptr<Mesh>> fileMap;

  }; // class Mesh

#if defined(_OPENGL)

  class OpenGLMesh : public Mesh {
    friend class Mesh;

    public:

      virtual ~OpenGLMesh();

      OpenGLMesh(OpenGLMesh& other) = delete;
      OpenGLMesh& operator=(OpenGLMesh& other) = delete;

      virtual void FinishBuilding();

      virtual void Draw() const;

    private:

      OpenGLMesh() = default;

      GLuint VAO = 0;
      GLuint VBO = 0;
      GLuint EBO = 0;

  }; // class OpenGLMesh

#elif defined(_DIRECTX)

  class DirectXMesh : public Mesh {
    friend class Mesh;

    public:

      virtual ~DirectXMesh();

      DirectXMesh(DirectXMesh& other) = delete;
      DirectXMesh& operator=(DirectXMesh& other) = delete;

      virtual void FinishBuilding();

      virtual void Draw() const;

    private:

      DirectXMesh() = default;

      // Handles to DirectX buffers holding the vertices and indices in the GPU.
      ID3D11Buffer *vertexBuffer = nullptr;
      ID3D11Buffer *indexBuffer = nullptr;

  }; // class DirectXMesh

#endif

} // namespace dg

namespace std {
  template<> struct hash<dg::Vertex> {
    typedef dg::Vertex argument_type;
    typedef dg::Vertex::hash_type result_type;
    result_type operator()(argument_type const& v) const noexcept {
      result_type h = 0;
      if (v.HasAllAttr(dg::Vertex::AttrFlag::POSITION)) {
        h ^= std::hash<glm::vec3>{}(v.data.position);
      }
      if (v.HasAllAttr(dg::Vertex::AttrFlag::NORMAL)) {
        h ^= std::hash<glm::vec3>{}(v.data.normal) << 1;
      }
      if (v.HasAllAttr(dg::Vertex::AttrFlag::TEXCOORD)) {
        h ^= std::hash<glm::vec2>{}(v.data.texCoord) << 2;
      }
      if (v.HasAllAttr(dg::Vertex::AttrFlag::TANGENT)) {
        h ^= std::hash<glm::vec3>{}(v.data.tangent) << 3;
      }
      return h;
    }
  };
}
