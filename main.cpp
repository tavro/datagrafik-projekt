#include "GL_utilities.h"
#include "MicroGlut.h"

#define MAIN
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"

void init(void)
{
	dumpInfo();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	printError("GL inits");

	glutRepeatingTimer(20.0f);

	printError("init arrays");
}

void display(void)
{
	printError("pre display");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	printError("display");
	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
	glutInitWindowSize(600, 600);
	glutCreateWindow ("Mixer");
	glutDisplayFunc(display); 
	init ();
	glutMainLoop();

	return 0;
}
