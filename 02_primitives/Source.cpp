#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <array>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h> 
#include <time.h>
#define PI 3.14159265359

using namespace std;

/*
A beadandót Kovács László készitette el (CW1FQM).
Ez az első beadandó feladat számitogépes grafika tárgyból (INBMM0635-17).
Az elkészitési dátum 2021.03.15.
A program ki rajzol egy kört ami x tengely mentén mozog abélak széltöl ablakszélig.
Ha megnyomnuk a 'W' gombot akkor irányt vált és függölegesen mozog tovább.
Ha az 'A' gombot nyomjuk meg akkor vissza áll vizszintes mozgásra.
Ha az 'R' gombot nyomjuk meg akkor egy véletlenszerüen generált szögben fog tovább patogni (ami lehet vizszintes és függöleges is).
Az 'U' gomb lenyomásával lehet a szimetria tengelnél kirajzolt vonalaz mozgatni felvelé a 'H' gomb lenyomásával pedig lefelé.
A kör szinezési határa nem változik a szimetria vonal mozgatásával mert az statikusan a fél ablakra van állitva.
A szinezés átmenetes a kordinátáknak megfelelöen és el van kerülve a fekete.
Az ablak mérte 600x600 px a main függvényben van beállitva.
A programhoz tartozik két shader fájl: vertexShader.glsl, fragmentShader.glsl
*/

GLuint VBO, VBO2;
GLuint VAO, VAO2;
GLfloat radius = 0.2;
GLfloat centerx = -0.6;
GLfloat centery = 0;
GLfloat novX = -0.005;
GLfloat novY = 0.005;
GLfloat multiX = 1.0;
GLfloat multiY = 0.0;
GLdouble updateFrequency = 0.01, lastUpdate;
float line_vertex[] ={
	-1,0, 1,0
};
GLfloat s_vertices[300];
GLuint renderingProgram;

void updateVertexData() {
	for (int i = 0; i < 100; i++) {
		s_vertices[3 * i] = centerx + radius * cos(i * (2 * PI / 100));
		s_vertices[3 * i + 1] = centery + radius * sin(i * (2 * PI / 100));
		s_vertices[3 * i + 2] = 0;
	}
}

void updateVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertex), line_vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

void printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

string readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";
	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

GLuint createShaderProgram() {
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;
	string vertShaderStr = readShaderSource("vertexShader.glsl");
	string fragShaderStr = readShaderSource("fragmentShader.glsl");
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);
	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}
	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}
	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printProgramLog(vfProgram);
	}
	glDeleteShader(vShader);
	glDeleteShader(fShader);
	return vfProgram;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_W:
			multiX = 0.0;
			multiY = 1.0;
			break;
		case GLFW_KEY_A:
			multiX = 1.0;
			multiY = 0.0;
			break;
		case GLFW_KEY_R:
			multiX = rand() % 4 + 1;
			multiY = rand() % 4 + 1;
			break;
		case GLFW_KEY_U:
			line_vertex[1] += 0.01;
			line_vertex[3] += 0.01;
			break;
		case GLFW_KEY_H:
			line_vertex[1] -= 0.01;
			line_vertex[3] -= 0.01;
			break;
		}
	}
}

void init() {
	renderingProgram = createShaderProgram();
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO2);
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//---------------------------------
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertex), line_vertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cleanUpScene() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(renderingProgram);
}

void display(GLFWwindow* window, double currentTime) {
	glClearColor(0.7, 0.7, 0.0, 0.8);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);
	glBindVertexArray(VAO);
	glPointSize(5.0);
	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 100, 1);
	if (currentTime - lastUpdate >= updateFrequency) {
		centerx += (novX*multiX);
		centery += (novY*multiY);
		if (centerx + radius > 1 - abs(novX) || centerx - radius < -1 + abs(novX))
			novX *= -1;
		if (centery + radius > 1 - abs(novY) || centery - radius < -1 + abs(novY))
			novY *= -1;
		lastUpdate = currentTime;
		updateVertexData();
		updateVBO();
	}
	glBindVertexArray(0);
	glBindVertexArray(VAO2);
	glLineWidth(3.0);
	glDrawArraysInstanced(GL_LINES, 0, 2, 2);	
	glBindVertexArray(0);
}

int main(void) {
	srand(time(NULL));
	updateVertexData();
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Labda pattog", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);
	init();
	lastUpdate = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	cleanUpScene();
	glfwTerminate();
	exit(EXIT_SUCCESS);
}