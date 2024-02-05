#include "GL_utilities.h"
#include "MicroGlut.h"

#define MAIN
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"
#include "SimpleGUI.h"

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <random>
#include <fstream>
#include <cstdlib>
#include <ctime> 
#include <vector>
#include <set>
#include <utility>
#include <algorithm>

std::vector<GLfloat> colors(8 * 3);
std::vector<GLubyte> indices(36);
std::vector<GLfloat> vertices(8 * 3);
void loadCubeData(const char* filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    for (int i = 0; i < 8*3; ++i) {
        file >> vertices[i];
    }

    for (int i = 0; i < 36; ++i) {
		int index;
		file >> index;
		indices[i] = static_cast<GLubyte>(index);
	}

    file.close();

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < 8*3; ++i) {
        colors[i] = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    }
}

struct PairComparator {
    bool operator() (const std::pair<GLubyte, GLubyte>& lhs, const std::pair<GLubyte, GLubyte>& rhs) const {
        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
    }
};

std::vector<GLubyte> lineIndices;
void generateLineIndices(const GLubyte* triangleIndices, size_t numTriangleIndices) {
    std::set<std::pair<GLubyte, GLubyte>, PairComparator> uniqueEdges;
    
    for (size_t i = 0; i < numTriangleIndices; i += 3) {
        GLubyte a = triangleIndices[i];
        GLubyte b = triangleIndices[i + 1];
        GLubyte c = triangleIndices[i + 2];
        
        uniqueEdges.insert(std::minmax({a, b}));
        uniqueEdges.insert(std::minmax({b, c}));
        uniqueEdges.insert(std::minmax({c, a}));
    }
    
	lineIndices.clear();
    for (const auto& edge : uniqueEdges) {
        lineIndices.push_back(edge.first);
        lineIndices.push_back(edge.second);
    }
}

GLfloat axisLinesColors[6*3];
GLfloat axisLinesVertices[6*3];
void loadAxisColorData(const char* filename) {
	std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

	for (int i = 0; i < 6*3; ++i) {
        file >> axisLinesColors[i];
    }

	file.close();
}

#include "constants.h"
mat4 projectionMatrix = mat4(
	2.0f * near / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
	0.0f, 2.0f * near / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
	0.0f, 0.0f, -(far + near) / (far - near), -2 * far * near / (far - near),
	0.0f, 0.0f, -1.0f, 0.0f
);

mat4 viewToWorld = lookAtv(
	vec3(0.0f, 0.0f, 0.5f), 
	vec3(0.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f)
);

mat4 translationMatrix;
mat4 rotationMatrix1, rotationMatrix2, rotationMatrix3;
mat4 mdlMatrix;

GLuint program;

float selectedX, selectedY, selectedZ;
float angle_x, angle_y, angle_z;

std::vector<int> selectedVertices;

unsigned int vertexArrayObjID;

unsigned int axisVertexBufferObjID;
unsigned int axisColorBufferObjID;
unsigned int lineIndexBufferObjID;
unsigned int vertexBufferObjID;
unsigned int indexBufferObjID;
unsigned int colorBufferObjID;

void init(void)
{
	dumpInfo();

	loadCubeData("./data/cubeData.txt");
	loadAxisColorData("./data/axisColorData.txt");

	generateLineIndices(indices.data(), indices.size());

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	program = loadShaders("cube.vert", "cube.frag");
	printError("init shader");

	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);

	glGenBuffers(1, &vertexBufferObjID);
	glGenBuffers(1, &indexBufferObjID);
	glGenBuffers(1, &colorBufferObjID);
	glGenBuffers(1, &lineIndexBufferObjID);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));
		
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Color"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Color"));
		
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLubyte), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBufferObjID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(GLubyte), lineIndices.data(), GL_STATIC_DRAW);

	translationMatrix = T(0, 0, -2);
	mdlMatrix = translationMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, viewToWorld.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, mdlMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	glGenBuffers(1, &axisVertexBufferObjID);
	glBindBuffer(GL_ARRAY_BUFFER, axisVertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisLinesVertices), axisLinesVertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &axisColorBufferObjID);
	glBindBuffer(GL_ARRAY_BUFFER, axisColorBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisLinesColors), axisLinesColors, GL_STATIC_DRAW);

	printError("init arrays");

	sgSetPosition(30, 30);
	sgSetTextColor(0,0,0);
	sgSetBackgroundColor(0.25, 0.25, 0.25, 0.5);
	sgSetFrameColor(0,0,0);
	//sgCreateStaticString(20, 20, "VERTEX CONTROLS");
	//sgCreateDisplayInt(-1, -1, "Selected Vertex: ", &selectedVertexIndex);
	//sgCreateDisplayFloat(-1, -1, "Vertex X: ", &selectedX);
	//sgCreateDisplayFloat(-1, -1, "Vertex Y: ", &selectedY);
	//sgCreateDisplayFloat(-1, -1, "Vertex Z: ", &selectedZ);
	sgCreateStaticString(20, 140, "SCENE CONTROLS");
	sgCreateDisplayFloat(-1, -1, "Angle X: ", &angle_x);
	sgCreateSlider(-1, -1, 200, &angle_x, -M_PI, M_PI);
	sgCreateDisplayFloat(-1, -1, "Angle Y: ", &angle_y);
	sgCreateSlider(-1, -1, 200, &angle_y, -M_PI, M_PI);
	sgCreateDisplayFloat(-1, -1, "Angle Z: ", &angle_z);
	sgCreateSlider(-1, -1, 200, &angle_z, -M_PI, M_PI);
}

bool inRange(long unsigned int i) {
	if (i < 0 || i >= vertices.size()) {
		printf("Vertex index out of bounds.\n");
		return false;
	}
	return true;
}

void updateAxisLines(int vertexIndex) {
	if(!inRange(vertexIndex)) return;

	float x = vertices[vertexIndex * 3];
	float y = vertices[vertexIndex * 3 + 1];
	float z = vertices[vertexIndex * 3 + 2];

	GLfloat updatedAxisVertices[6*3] = {
		x, y, z, x + 1.0f, y, z,
		x, y, z, x, y + 1.0f, z,
		x, y, z, x, y, z + 1.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, axisVertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(updatedAxisVertices), updatedAxisVertices, GL_DYNAMIC_DRAW);
}

void getCurrentVertexPosition(int vertexIndex, float* x, float* y, float* z) {
	if(!inRange(vertexIndex)) return;
		
	int arrayIndex = vertexIndex * 3;
	*x = vertices[arrayIndex];
	*y = vertices[arrayIndex + 1];
	*z = vertices[arrayIndex + 2];
}

void moveVertex(int vertexIndex, unsigned char dir, float amount) {
	if(!inRange(vertexIndex)) return;
		
	int arrayIndex = vertexIndex * 3;
	getCurrentVertexPosition(vertexIndex, &selectedX, &selectedY, &selectedZ);

	if(dir == 'w') {
		vertices[arrayIndex + 1] = vertices[arrayIndex + 1] + amount;
	}
	else if(dir == 's') {
		vertices[arrayIndex + 1] = vertices[arrayIndex + 1] - amount;
	}

	if(dir == 'a') {
		vertices[arrayIndex] = vertices[arrayIndex] + amount;
	}
	else if(dir == 'd') {
		vertices[arrayIndex] = vertices[arrayIndex] - amount;
	}

	if(dir == 'q') {
		vertices[arrayIndex + 2] = vertices[arrayIndex + 2] + amount;
	}
	else if(dir == 'e') {
		vertices[arrayIndex + 2] = vertices[arrayIndex + 2] - amount;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
}

void moveVertices(unsigned char dir, float amount) {
	for(int i : selectedVertices) {
		moveVertex(i, dir, amount);
	}
}

void updateVertexPosition(int vertexIndex, float x, float y, float z) {
	if(!inRange(vertexIndex)) return;
		
	int arrayIndex = vertexIndex * 3;

	vertices[arrayIndex] = x;
	vertices[arrayIndex + 1] = y;
	vertices[arrayIndex + 2] = z;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
}

void display(void)
{
	printError("pre display");

	// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
		
	rotationMatrix1 = Rx(angle_x);
	rotationMatrix2 = Ry(angle_y);
	rotationMatrix3 = Rz(angle_z);
	mdlMatrix = translationMatrix * rotationMatrix1 * rotationMatrix2 * rotationMatrix3;
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, mdlMatrix.m);

	glBindVertexArray(vertexArrayObjID);
		
	// draw cube
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_BYTE, 0);

	// setup line drawing
	glUniform1i(glGetUniformLocation(program, "useUniformColor"), GL_TRUE);
	GLfloat lineColor[3] = {0.0, 0.0, 0.0};
	glUniform3fv(glGetUniformLocation(program, "uniformColor"), 1, lineColor);

	// draw lines
	glLineWidth(2.0f);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBufferObjID);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_BYTE, 0);

	for (long unsigned int i = 0; i < vertices.size()/3; ++i) {
		if (std::find(selectedVertices.begin(), selectedVertices.end(), i) != selectedVertices.end()) {
			glPointSize(20.0f);
			GLfloat selectedColor[3] = {1.0, 0.0, 0.0};
			glUniform3fv(glGetUniformLocation(program, "uniformColor"), 1, selectedColor);
		} else {
			glPointSize(10.0f);
			GLfloat pointColor[3] = {1.0, 0.5, 0.0};
			glUniform3fv(glGetUniformLocation(program, "uniformColor"), 1, pointColor);
		}
		glDrawArrays(GL_POINTS, i, 1);
	}
	glUniform1i(glGetUniformLocation(program, "useUniformColor"), GL_FALSE);

	GLint posAttribLocation = glGetAttribLocation(program, "in_Position");
	GLint colorAttribLocation = glGetAttribLocation(program, "in_Color");

	if (!selectedVertices.empty()) {
		updateAxisLines(selectedVertices[selectedVertices.size()-1]); // TODO: Place axis in between all points

		glEnableVertexAttribArray(posAttribLocation);
		glBindBuffer(GL_ARRAY_BUFFER, axisVertexBufferObjID);
		glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(colorAttribLocation);
		glBindBuffer(GL_ARRAY_BUFFER, axisColorBufferObjID);
		glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_LINES, 0, 6);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttribLocation);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colorAttribLocation);

	sgDraw();

	printError("display");
		
	glutSwapBuffers();
}

struct Ray {
	vec3 origin;
	vec3 direction;
};

float length(const vec3& v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Ray createRayFromScreenCoordinates(int screenX, int screenY) {
	float screenWidth = glutGet(GLUT_WINDOW_WIDTH);
	float screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

	float ndcX = (screenX / screenWidth) * 2.0f - 1.0f;
	float ndcY = 1.0f - (screenY / screenHeight) * 2.0f;

	vec4 ndcPos = vec4(ndcX, ndcY, -1.0f, 1.0f);
	vec4 clipPos = inverse(projectionMatrix) * ndcPos;
	vec4 worldPos = inverse(viewToWorld) * clipPos;

	vec3 rayOrigin = vec3(worldPos.x / worldPos.w, worldPos.y / worldPos.w, worldPos.z / worldPos.w);
	vec3 rayDirection = normalize(rayOrigin - vec3(0.0f, 0.0f, 0.5f)); // camera position

	return Ray{rayOrigin, rayDirection};
}

bool shift_down = false;
bool locked = false;
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !locked) {
		Ray ray = createRayFromScreenCoordinates(x, y);

		float closestDistance = std::numeric_limits<float>::max();
		int closestVertexIndex = -1;

		for (long unsigned int i = 0; i < vertices.size(); ++i) {
			float vertexWorldX, vertexWorldY, vertexWorldZ;
			getCurrentVertexPosition(i, &vertexWorldX, &vertexWorldY, &vertexWorldZ);

			vec4 vertexPos = vec4(vertexWorldX, vertexWorldY, vertexWorldZ, 1.0f);
			vertexPos = mdlMatrix * vertexPos;

			vec3 vertexPosition = vec3(vertexPos.x, vertexPos.y, vertexPos.z);

			vec3 rayToVertex = vertexPosition - ray.origin;
			float t = dot(rayToVertex, ray.direction);

			if (t > 0) {
				vec3 closestPoint = ray.origin + ray.direction * t;

				float distance = length(closestPoint - vertexPosition);

				if (distance < closestDistance) {
					closestDistance = distance;
					closestVertexIndex = i;
				}
			}
		}

		if(!shift_down) {
			selectedVertices.clear();
		}
		
		if(closestVertexIndex != -1) {
			selectedVertices.push_back(closestVertexIndex);
		}
		//getCurrentVertexPosition(selectedVertexIndex, &selectedX, &selectedY, &selectedZ);
		glutPostRedisplay();
	}
	sgMouse(state, x, y);
}

void extrude(float distance) {
    std::vector<float> extrusionDirection = {0, 0, 1};
    int newVertexStartIndex = vertices.size() / 3;

    for (int selectedIndex : selectedVertices) {
        int vertexPosIndex = selectedIndex * 3;
        float newX = vertices[vertexPosIndex] + extrusionDirection[0] * distance;
        float newY = vertices[vertexPosIndex + 1] + extrusionDirection[1] * distance;
        float newZ = vertices[vertexPosIndex + 2] + extrusionDirection[2] * distance;

        vertices.push_back(newX);
        vertices.push_back(newY);
        vertices.push_back(newZ);

        for (int j = 0; j < 3; ++j) {
            colors.push_back(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
        }
    }

    for (size_t i = 0; i < selectedVertices.size(); ++i) {
        int originalIndex = selectedVertices[i];
        int newIndex = newVertexStartIndex + i;

        indices.push_back(originalIndex);
        indices.push_back((i + 1) < selectedVertices.size() ? newVertexStartIndex + i + 1 : newVertexStartIndex);
        indices.push_back(newIndex);

        indices.push_back(originalIndex);
        indices.push_back((i + 1) < selectedVertices.size() ? selectedVertices[i + 1] : selectedVertices[0]);
        indices.push_back((i + 1) < selectedVertices.size() ? newVertexStartIndex + i + 1 : newVertexStartIndex);
    }

	generateLineIndices(indices.data(), indices.size());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexBufferObjID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(GLubyte), lineIndices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), &colors[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

void deleteFace() {
	// TODO: Implement
}

void saveToFile(std::string path) {
	// TODO: Implement
}

void loadFromFile(std::string path) {
	// TODO: Implement
}

void keys(unsigned char key, int x, int y) 
{
	if(key == 'w' || key == 's' || key == 'a' || key == 'd' || key == 'q' || key == 'e') {
		moveVertices(key, 0.025f);
	}
	else if(key == 'l') {
		locked = !locked;
	}
	else if (key == 'E') {
		if(selectedVertices.size() == 3) {
			extrude(0.5f);
		}
		selectedVertices.clear();
	}
}

void specialKeys(unsigned char key, int x, int y) {
    switch (key) {
        case 14: // shift
            shift_down = true;
            break;
    }
}

void specialKeysUp(unsigned char key, int x, int y) {
    switch (key) {
        case 14:
            shift_down = false;
            break;
    }
}

void mousedrag(int x, int y)
{
	sgMouseDrag(x, y);
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
	glutMouseFunc(mouse);
	glutKeyboardFunc(keys);
	glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
	glutMotionFunc(mousedrag);
	init ();
	glutMainLoop();

	return 0;
}
