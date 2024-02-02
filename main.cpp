#include "GL_utilities.h"
#include "MicroGlut.h"

#define MAIN
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"

#include <math.h>

GLfloat vertices[36*3] = {
	-0.5,-0.5,-0.5,	// 0
	-0.5,0.5,-0.5,	// 3
	0.5,0.5,-0.5,	// 2
	-0.5,-0.5,-0.5,	// 0
	0.5,0.5,-0.5,	// 2
	0.5,-0.5,-0.5,	// 1

	0.5,0.5,-0.5,	// 2
	-0.5,0.5,-0.5,	// 3
	-0.5,0.5,0.5,	// 7
	0.5,0.5,-0.5,	// 2
	-0.5,0.5,0.5,	// 7
	0.5,0.5,0.5,	// 6

	-0.5,-0.5,-0.5,	// 0
	-0.5,-0.5,0.5,	// 4
	-0.5,0.5,0.5,	// 7
	-0.5,-0.5,-0.5,	// 0
	-0.5,0.5,0.5,	// 7
	-0.5,0.5,-0.5,	// 3

	0.5,-0.5,-0.5,	// 1
	0.5,0.5,-0.5,	// 2
	0.5,0.5,0.5,	// 6
	0.5,-0.5,-0.5,	// 1
	0.5,0.5,0.5,	// 6
	0.5,-0.5,0.5,	// 5

	-0.5,-0.5,0.5,	// 4
	0.5,-0.5,0.5,	// 5
	0.5,0.5,0.5,	// 6
	-0.5,-0.5,0.5,	// 4
	0.5,0.5,0.5,	// 6
	-0.5,0.5,0.5,	// 7

	-0.5,-0.5,-0.5,	// 0
	0.5,-0.5,-0.5,	// 1
	0.5,-0.5,0.5,	// 5
	-0.5,-0.5,-0.5,	// 0
	0.5,-0.5,0.5,	// 5
	-0.5,-0.5,0.5	// 4						
};

GLfloat colors[36*3] = {
	1.0, 0.0, 0.0,	// Red
	1.0, 0.0, 0.0,	// Red
	1.0, 0.0, 0.0,	// Red
	1.0, 0.0, 0.0,	// Red
	1.0, 0.0, 0.0,	// Red
	1.0, 0.0, 0.0,	// Red

	0.0, 1.0, 0.0,	// Green
	0.0, 1.0, 0.0,	// Green
	0.0, 1.0, 0.0,	// Green
	0.0, 1.0, 0.0,	// Green
	0.0, 1.0, 0.0,	// Green
	0.0, 1.0, 0.0,	// Green

	0.0, 0.0, 1.0,	// Blue
	0.0, 0.0, 1.0,	// Blue
	0.0, 0.0, 1.0,	// Blue
	0.0, 0.0, 1.0,	// Blue
	0.0, 0.0, 1.0,	// Blue
	0.0, 0.0, 1.0,	// Blue

	0.0, 1.0, 1.0,	// Cyan
	0.0, 1.0, 1.0,	// Cyan
	0.0, 1.0, 1.0,	// Cyan
	0.0, 1.0, 1.0,	// Cyan
	0.0, 1.0, 1.0,	// Cyan
	0.0, 1.0, 1.0,	// Cyan

	1.0, 0.0, 1.0,	// Magenta
	1.0, 0.0, 1.0,	// Magenta
	1.0, 0.0, 1.0,	// Magenta
	1.0, 0.0, 1.0,	// Magenta
	1.0, 0.0, 1.0,	// Magenta
	1.0, 0.0, 1.0,	// Magenta

	1.0, 1.0, 0.0,	// Yellow
	1.0, 1.0, 0.0,	// Yellow
	1.0, 1.0, 0.0,	// Yellow
	1.0, 1.0, 0.0,	// Yellow
	1.0, 1.0, 0.0,	// Yellow
	1.0, 1.0, 0.0,	// Yellow
};

GLfloat rotationMatrix[] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f 
};

GLfloat rotationMatrix2[] = {	
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f 
};

GLfloat translationMatrix[] = {	
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, -2.0f,
	0.0f, 0.0f, 0.0f, 1.0f 
};

#define near 1.0
#define far 30.0
#define right 1.0
#define left -1.0
#define top 1.0
#define bottom -1.0
GLfloat projectionMatrix[] = {	
	2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
	0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
	0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
	0.0f, 0.0f, -1.0f, 0.0f 
};

unsigned int vertexArrayObjID;
GLuint program;

void init(void)
{
	unsigned int vertexBufferObjID;
	unsigned int colorBufferObjID;

	dumpInfo();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	program = loadShaders("cube.vert", "cube.frag");
	printError("init shader");

	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);

	glGenBuffers(1, &vertexBufferObjID);
	glGenBuffers(1, &colorBufferObjID);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 36*3*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 36*3*sizeof(GLfloat), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Color"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Color"));

	glUniformMatrix4fv(glGetUniformLocation(program, "translationMatrix"), 1, GL_TRUE, translationMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrix"), 1, GL_TRUE, rotationMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);

	printError("init arrays");
}

float angle = 0.0f;
float speed = 0.05f;
void display(void)
{
	printError("pre display");
	angle += speed;

	// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	rotationMatrix2[0] = cos(angle/5.0);
	rotationMatrix2[1] = -sin(angle/5.0);
	rotationMatrix2[4] = sin(angle/5.0);
	rotationMatrix2[5] = cos(angle/5.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrix2"), 1, GL_TRUE, rotationMatrix2);
	rotationMatrix[5] = cos(angle);
	rotationMatrix[6] = -sin(angle);
	rotationMatrix[9] = sin(angle);
	rotationMatrix[10] = cos(angle);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrix"), 1, GL_TRUE, rotationMatrix);

	glBindVertexArray(vertexArrayObjID);
	glDrawArrays(GL_TRIANGLES, 0, 36*3);

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
	glutRepeatingTimer(20);
	glutDisplayFunc(display); 
	init ();
	glutMainLoop();

	return 0;
}
