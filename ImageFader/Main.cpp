#ifdef _MSC_VER
	// This prevents Windows from opening the stdout command window.
	#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#include <iostream>
#include <cstdlib>
#include <GL/glew.h>
#include <GL/glut.h>

bool make_resources();
void update_fade_factor();
void render();

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(400, 300);
    glutCreateWindow("OpenGL Image Fader");
    glutDisplayFunc(&render);
    glutIdleFunc(&update_fade_factor);

	glewInit();
    if (!GLEW_VERSION_2_0) {
		std::cerr << "OpenGL 2.0 not available." << std::endl;
        return 1;
    }

    if (!make_resources()) {
		std::cerr << "Failed to load resources." << std::endl;
        return 1;
    }

    glutMainLoop();

	return 0;
}

bool make_resources() {
	return true;
}

void update_fade_factor() {

}

void render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();	
}
