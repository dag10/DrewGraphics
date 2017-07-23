#ifdef _MSC_VER
	// This prevents Windows from opening the stdout command window.
	#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#include <iostream>
#include <cstdlib>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

// Types

struct {
	GLuint vertex_buffer;
	GLuint element_buffer;
	GLuint textures[2];

	// TODO: Add fields for shader objects.
} g_resources;

// Scene data

static const GLfloat g_vertex_buffer_data[] = { 
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f,
};

static const GLushort g_element_buffer_data[] = {
	0, 1, 2, 3,
};

// Functions

// util.c
void *read_tga(const char *filename, int *width, int *height);

bool make_resources();
void update_fade_factor();
void render();
GLuint make_buffer(GLenum target, const void *buffer_data, GLsizei buffer_size);
GLuint make_texture(const char *filename);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(400, 300);
    glutCreateWindow("OpenGL Image Fader");
    glutDisplayFunc(&render);
    glutIdleFunc(&update_fade_factor);

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

	g_resources.textures[0] = make_texture("image1.tga");
	g_resources.textures[1] = make_texture("image2.tga");

	if (!g_resources.textures[0] || !g_resources.textures[1]) {
		return false;
	}

	return true;
}

void update_fade_factor() {
	// TODO: Update fade amount.
}

void render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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

GLuint make_texture(const char *filename) {
	GLuint texture;
	int width;
	int height;

	void *pixels = read_tga(filename, &width, &height);
	if (!pixels) {
		return 0;
	}

	// Generate one new texture handle.
	glGenTextures(1, &texture);

	// Reference this texture through a particular target.
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set parameters for this texture.
	// It will use linear interpolation, and clamp out-of-bound
	// values to the nearest edge.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Allocate the texture space and transfer the pixel data.
	glTexImage2D(
		GL_TEXTURE_2D,
		0, // Level of detail
		GL_RGB8, // Internal format
		width,
		height,
		0, // Border
		GL_BGR, // External format
		GL_UNSIGNED_BYTE, // Type
		pixels
	);

	free(pixels);
	return texture;
}
