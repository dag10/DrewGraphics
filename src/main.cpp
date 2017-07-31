#include <iostream>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "Texture.h"
#include "Shader.h"
#include "Exceptions.h"
#include <GLUT/glut.h>
#include <glm/glm/mat4x4.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

/* Types */

struct {
  GLuint vertex_buffer;
  GLuint element_buffer;
  dg::Texture textures[2];
  dg::Shader shader;

  GLfloat elapsed_time;
} g_resources;

/* Scene data */

static const GLfloat g_vertex_buffer_data[] = { 
  -1.0f, -1.0f, 0.0f, 1.0f,
  1.0f, -1.0f, 0.0f, 1.0f,
  -1.0f,  1.0f, 0.0f, 1.0f,
  1.0f,  1.0f, 0.0f, 1.0f,
};

static const GLushort g_element_buffer_data[] = {
  0, 1, 2, 3,
};

/* Functions */

bool make_resources();
void update();
void render();
GLuint make_buffer(GLenum target, const void *buffer_data, GLsizei buffer_size);

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(400, 300);
  glutCreateWindow("OpenGL Image Fader");
  glutDisplayFunc(&render);
  glutIdleFunc(&update);

  if (!make_resources()) {
    std::cerr << "Failed to load resources." << std::endl;
    return 1;
  }

  glutMainLoop();

  return 0;
}

bool make_resources() {
  // Make buffers
  g_resources.vertex_buffer = make_buffer(
      GL_ARRAY_BUFFER,
      g_vertex_buffer_data,
      sizeof(g_vertex_buffer_data)
      );
  g_resources.element_buffer = make_buffer(
      GL_ELEMENT_ARRAY_BUFFER,
      g_element_buffer_data,
      sizeof(g_element_buffer_data)
      );

  // Make textures
  try {
    g_resources.textures[0] = dg::Texture::FromPath(
        "assets/textures/image1.tga");
    g_resources.textures[1] = dg::Texture::FromPath(
        "assets/textures/image2.tga");
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }

  // Make shaders
  
  try {
    g_resources.shader = dg::Shader::FromFiles(
        "assets/shaders/frustum-rotation.v.glsl",
        "assets/shaders/hello-gl.f.glsl");
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }

  return true;
}

void update() {
  int milliseconds = glutGet(GLUT_ELAPSED_TIME);
  g_resources.elapsed_time = (GLfloat)milliseconds * 0.001f;
  glutPostRedisplay();
}

glm::mat4x4 create_vp() {
  glm::vec3 eyePos(0.0f, 5.f + 5.f * sin(glm::radians(g_resources.elapsed_time * 180)), 10.0f);
  glm::mat4x4 view = glm::lookAt(eyePos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
  view = glm::rotate(view, glm::radians(g_resources.elapsed_time * 60.f), glm::vec3(0.f, 1.f, 0.f));

  float aspect = 4.0f / 3.0f;
  glm::mat4x4 projection = glm::perspective(
      glm::radians(60.0f), 1.0f / aspect, 0.1f, 100.0f);

  return projection * view;
}

void drawPlane(glm::mat4x4 pv, glm::mat4x4 m) {
  glm::mat4x4 mvp = pv * glm::translate(
      glm::mat4x4(), glm::vec3(0.f, 0.f, 0.f)) * m;
  g_resources.shader.SetMat4("MATRIX_MVP", mvp);

  glDrawElements(
      GL_TRIANGLE_STRIP,  /* mode */
      4,                  /* count */
      GL_UNSIGNED_SHORT,  /* type */
      (void*)0            /* element array buffer offset */
      );
}

void render() {
  // Clear the back buffer.
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  // Specify the shader
  g_resources.shader.Use();

  // Set the uniforms
  g_resources.shader.SetFloat("elapsed_time", g_resources.elapsed_time);
  g_resources.shader.SetTexture(0, "textures[0]", g_resources.textures[0]);
  g_resources.shader.SetTexture(1, "textures[1]", g_resources.textures[1]);

  // Set vertex array
  GLuint positionAttribute = \
      g_resources.shader.GetAttributeLocation("position");
  glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
  glVertexAttribPointer(
      positionAttribute,                        // attribute
      4,                                // size
      GL_FLOAT,                         // type
      GL_FALSE,                         // normalized?
      sizeof(GLfloat)*4,                // stride
      (void*)0                          // array buffer offset
      );
  glEnableVertexAttribArray(positionAttribute);

  // Set index array
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);

  // Initial transformation matrices.
  glm::mat4x4 xfRotation = glm::rotate(
      glm::mat4x4(),
      glm::radians(g_resources.elapsed_time * 90),
      glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4x4 pv = create_vp();

  // Draw the left model.
  g_resources.shader.SetBool("invertTexture", false);
  drawPlane(
      pv,
      glm::translate(glm::mat4x4(), glm::vec3(-3.f, 0.f, 0.f)) * xfRotation);

  // Draw the center model.
  g_resources.shader.SetBool("invertTexture", true);
  drawPlane(
      pv,
      glm::translate(glm::mat4x4(), glm::vec3(0.f, 0.f, 0.f)) * xfRotation);

  // Draw the right model.
  g_resources.shader.SetBool("invertTexture", false);
  drawPlane(
      pv,
      glm::translate(glm::mat4x4(), glm::vec3(3.f, 0.f, 0.f)) * xfRotation);

  // Clean up
  glDisableVertexAttribArray(positionAttribute);

  // Present rendered buffer to screen
  glutSwapBuffers();	
}

GLuint make_buffer(
    GLenum target, const void *buffer_data,
    GLsizei buffer_size) {
  GLuint buffer;

  // Generate one new buffer handle.
  glGenBuffers(1, &buffer);

  // Reference this buffer through a particular target.
  glBindBuffer(target, buffer);

  // Allocate the buffer size, with a usage hint indicating
  // that we only write the data once, and only the GPU accesses it.
  glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);

  return buffer;
}

