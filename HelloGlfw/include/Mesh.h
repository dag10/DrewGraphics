//
//  Mesh
//

#pragma once

#include <GLUT/glut.h>
#include <glm/glm.hpp>
#include <memory>
#include "Shader.h"

namespace dg {

  // Copy is disabled, only moves are allowed. This prevents us
  // from leaking or redeleting the openGL resources.
  class Mesh {

    public:
      static std::shared_ptr<Mesh> Cube;
      static std::shared_ptr<Mesh> Quad;

      static void CreatePrimitives();

      Mesh() = default;
      Mesh(Mesh& other) = delete;
      Mesh(Mesh&& other);
      ~Mesh();
      Mesh& operator=(Mesh& other) = delete;
      Mesh& operator=(Mesh&& other);
      friend void swap(Mesh& first, Mesh& second); // nothrow

      void Use() const;
      void Draw() const;
      void FinishUsing() const;

    private:
      static std::unique_ptr<Mesh> CreateCube();
      static std::unique_ptr<Mesh> CreateQuad();

      GLenum drawMode = 0;
      GLsizei drawCount = 0;
      GLuint VAO = 0;
      GLuint VBO = 0;

  }; // class Mesh

} // namespace dg
