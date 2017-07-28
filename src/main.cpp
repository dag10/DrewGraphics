#include <iostream>
#include <memory>
#include <cmath>
#include <cstdlib>
#include "file_util.h"
#include "Texture.h"
#include "Exceptions.h"
#include <GLUT/glut.h>

/* Types */

// These function pointer types are defined by GLEW, which we don't use on Mac.
typedef std::function<void(GLuint, GLsizei, GLsizei *, GLchar *)> PFNGLGETSHADERINFOLOGPROC;
typedef std::function<void(GLuint, GLenum, GLint *)> PFNGLGETSHADERIVPROC;

struct {
	GLuint vertex_buffer;
	GLuint element_buffer;
		std::shared_ptr<dg::Texture> textures[2];
    
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
    
    struct {
        GLint elapsed_time;
        GLint textures[2];
    } uniforms;
    
    struct {
        GLint position;
    } attributes;
    
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
void idle();
void render();
GLuint make_buffer(GLenum target, const void *buffer_data, GLsizei buffer_size);
GLuint make_shader(GLenum type, const char *filename);
GLuint make_program(GLuint vertex_shader, GLuint fragment_shader);


int main(int argc, char **argv) {

	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(400, 300);
    glutCreateWindow("OpenGL Image Fader");
    glutDisplayFunc(&render);
    glutIdleFunc(&idle);

#ifndef __APPLE__
    glewInit();
    if (!GLEW_VERSION_2_0) {
		std::cerr << "OpenGL 2.0 not available." << std::endl;
        return 1;
    }
#endif

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
    
    g_resources.vertex_shader = make_shader(
        GL_VERTEX_SHADER, "assets/shaders/frustum-rotation.v.glsl");
    
    if (g_resources.vertex_shader == 0) {
        return 0;
    }
    
    g_resources.fragment_shader = make_shader(
        GL_FRAGMENT_SHADER, "assets/shaders/hello-gl.f.glsl");
    
    if (g_resources.fragment_shader == 0) {
        return false;
    }
    
    g_resources.program = make_program(
        g_resources.vertex_shader, g_resources.fragment_shader);
    
    if (g_resources.program == 0) {
        return false;
    }
    
    // Get handles to shader variables
    
    g_resources.uniforms.elapsed_time =
        glGetUniformLocation(g_resources.program, "elapsed_time");
    g_resources.uniforms.textures[0] =
        glGetUniformLocation(g_resources.program, "textures[0]");
    g_resources.uniforms.textures[1] =
        glGetUniformLocation(g_resources.program, "textures[1]");
    
    g_resources.attributes.position =
        glGetAttribLocation(g_resources.program, "position");

	return true;
}

void idle() {
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);
    g_resources.elapsed_time = (GLfloat)milliseconds * 0.001f;
    glutPostRedisplay();
}

void render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Specify the shader
    glUseProgram(g_resources.program);
    
    // Set the uniforms
    glUniform1f(g_resources.uniforms.elapsed_time, g_resources.elapsed_time);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_resources.textures[0]->GetHandle());
    glUniform1i(g_resources.uniforms.textures[0], 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_resources.textures[1]->GetHandle());
    glUniform1i(g_resources.uniforms.textures[1], 1);
    
    // Set vertex array
    glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
    glVertexAttribPointer(
        g_resources.attributes.position,  // attribute
        4,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        sizeof(GLfloat)*4,                // stride
        (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(g_resources.attributes.position);

    // Set index array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);
    
    // Draw call!
    glDrawElements(
        GL_TRIANGLE_STRIP,  /* mode */
        4,                  /* count */
        GL_UNSIGNED_SHORT,  /* type */
        (void*)0            /* element array buffer offset */
    );
    
    // Clean up
    glDisableVertexAttribArray(g_resources.attributes.position);
    
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

void show_info_log(
        GLuint object, PFNGLGETSHADERIVPROC glGet__iv,
        PFNGLGETSHADERINFOLOGPROC glGet__InfoLog) {
    GLint log_length;
    
    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
	char *log = new char[log_length];
    glGet__InfoLog(object, log_length, NULL, log);
    std::cerr << log << std::endl;
	delete[] log;
}

GLuint make_shader(GLenum type, const char *filename) {
    GLint length;
    GLchar *source = (GLchar*)file_contents(filename, &length);
    GLuint shader;
    GLint shader_ok;
    
    if (!source) return 0;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, &length);
	delete source;
    glCompileShader(shader);
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if (!shader_ok) {
        std::cerr << "Failed to compile " << filename << ":" << std::endl;
        show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint make_program(GLuint vertex_shader, GLuint fragment_shader) {
    GLint program_ok;
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        std::cerr << "Failed to link shader program:" << std::endl;
        show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}
