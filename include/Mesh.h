//
//  Mesh.h
//

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <Shader.h>

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL resources.
  class Mesh {

    public:
      static std::shared_ptr<Mesh> Cube;
      static std::shared_ptr<Mesh> MappedCube;
      static std::shared_ptr<Mesh> Quad;
      static std::shared_ptr<Mesh> Cylinder;

      static void CreatePrimitives();

      Mesh();
      Mesh(Mesh& other) = delete;
      Mesh(Mesh&& other);
      ~Mesh();
      Mesh& operator=(Mesh& other) = delete;
      Mesh& operator=(Mesh&& other);
      friend void swap(Mesh& first, Mesh& second); // nothrow

      void Use() const;
      void Draw() const;
      void FinishUsing() const;

      enum {
        ATTR_POSITION  = 0,
        ATTR_NORMAL    = 1,
        ATTR_TEX_COORD = 2,
        ATTR_TANGENT   = 3,

        ATTR_MAX, // Bookend
      };

    private:
      static std::unique_ptr<Mesh> CreateCube();
      static std::unique_ptr<Mesh> CreateMappedCube();
      static std::unique_ptr<Mesh> CreateQuad();
      static std::unique_ptr<Mesh> CreateCylinder(
          int radialDivisions, int heightDivisions);

      GLenum drawMode = 0;
      GLsizei drawCount = 0;
      GLuint VAO = 0;
      GLuint VBO = 0;
      bool useAttribute[ATTR_MAX];

  }; // class Mesh

} // namespace dg
