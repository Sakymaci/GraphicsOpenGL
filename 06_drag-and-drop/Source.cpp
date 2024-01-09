#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

static std::vector<glm::vec3> myPoints = {
	glm::vec3(-0.5f, -0.5f, 0.0f),
	glm::vec3( 0.5f,  0.5f, 0.0f),
};

/* Vertex buffer objektum �s vertex array objektum az adatt�rol�shoz.*/
#define numVBOs 1
#define numVAOs 1
GLuint VBO[numVBOs];
GLuint VAO[numVAOs];

GLuint renderingProgram;

int window_width	= 600;
int window_height	= 600;

GLint dragged		= -1;

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

	// Shader program objektum l�trehoz�sa. Elt�roljuk az ID �rt�ket.
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

GLfloat dist2(glm::vec3 P1, glm::vec3 P2) {
	GLfloat dx = P1.x - P2.x;
	GLfloat dy = P1.y - P2.y;

	return dx * dx + dy * dy;
}

GLint getActivePoint(vector<glm::vec3> p, GLfloat sensitivity, GLfloat x, GLfloat y) {
	GLfloat		s = sensitivity * sensitivity;
	GLint		size = p.size();
	GLfloat		xNorm = x / (window_width / 2) - 1.0f;
	GLfloat		yNorm = y / (window_height / 2) - 1.0f;
	glm::vec3	P = glm::vec3(xNorm, yNorm, 0.0f);

	for (GLint i = 0; i < size; i++)
		if (dist2(p[i], P) < s)
			return i;

	return -1;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	if (dragged >= 0) {
		GLfloat	xNorm = xPos / (window_width / 2) - 1.0f;
		GLfloat	yNorm = (window_height - yPos) / (window_height / 2) - 1.0f;

		myPoints.at(dragged).x = xNorm;
		myPoints.at(dragged).y = yNorm;

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, myPoints.size() * sizeof(glm::vec3), myPoints.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double	x, y;

		glfwGetCursorPos(window, &x, &y);
		dragged = getActivePoint(myPoints, 0.1f, x, window_height - y);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		dragged = -1;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	window_width	= width;
	window_height	= height;

	glViewport(0, 0, width, height);
}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram();

	/* L�trehozzuk a sz�ks�ges Vertex buffer �s vertex array objektumot. */
	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);

	/* T�pus meghat�roz�sa: a GL_ARRAY_BUFFER neves�tett csatol�ponthoz kapcsoljuk a buffert (ide ker�lnek a vertex adatok). */
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	/* M�soljuk az adatokat a pufferbe! Megadjuk az aktu�lisan csatolt puffert,  azt hogy h�ny b�jt adatot m�solunk,
	a m�soland� adatot, majd a feldolgoz�s m�dj�t is meghat�rozzuk: most az adat nem v�ltozik a felt�lt�s ut�n */
	glBufferData(GL_ARRAY_BUFFER, myPoints.size() * sizeof(glm::vec3), myPoints.data(), GL_STATIC_DRAW);

	/* Csatoljuk a vertex array objektumunkat a konfigur�l�shoz. */
	glBindVertexArray(VAO[0]);

	/* Ezen adatok szolg�lj�k a 0 index� vertex attrib�tumot (itt: poz�ci�).
	Els�k�nt megadjuk ezt az azonos�t�sz�mot.
	Ut�na az attrib�tum m�ret�t (vec3, l�ttuk a shaderben).
	Harmadik az adat t�pusa.
	Negyedik az adat normaliz�l�sa, ez maradhat FALSE jelen p�ld�ban.
	Az attrib�tum �rt�kek hogyan k�vetkeznek egym�s ut�n? Milyen l�p�sk�z ut�n tal�lom a k�vetkez� vertex adatait?
	V�g�l megadom azt, hogy honnan kezd�dnek az �rt�kek a pufferben. Most r�gt�n, a legelej�t�l veszem �ket.*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	/* Enged�lyezz�k az im�nt defini�lt 0 index� attrib�tumot. */
	glEnableVertexAttribArray(0);

	/* Lev�lasztjuk a vertex array objektumot �s a puffert is.*/
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	// aktiv�ljuk a shader-program objektumunkat.
	glUseProgram(renderingProgram);
}

/** A jelenet�nk ut�ni takar�t�s. */
void cleanUpScene() {
	/** T�r�lj�k a vertex puffer �s vertex array objektumokat. */
	glDeleteVertexArrays(numVAOs, VAO);
	glDeleteBuffers(numVBOs, VBO);

	/** T�r�lj�k a shader programot. */
	glDeleteProgram(renderingProgram);
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT); // fontos lehet minden egyes alkalommal t�r�lni!

	/*Csatoljuk a vertex array objektumunkat. */
	glBindVertexArray(VAO[0]);

	glLineWidth(4.0f);
	glDrawArrays(GL_LINES, 0, myPoints.size());

	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, myPoints.size());

	/* Lev�lasztjuk, nehogy b�rmilyen �rt�k fel�l�r�djon.*/
	glBindVertexArray(0);
}

int main(void) {
	/* Pr�b�ljuk meg inicializ�lni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/* A k�v�nt OpenGL verzi� (4.3) */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/* Pr�b�ljuk meg l�trehozni az ablakunkat. */
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Drag and Drop", NULL, NULL);

	/* V�lasszuk ki az ablakunk OpenGL kontextus�t, hogy haszn�lhassuk. */
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	/* Incializ�ljuk a GLEW-t, hogy el�rhet�v� v�ljanak az OpenGL f�ggv�nyek. */
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	/* Az alkalmaz�shoz kapcsol�d� el�k�sz�t� l�p�sek, pl. hozd l�tre a shader objektumokat. */
	init(window);

	while (!glfwWindowShouldClose(window)) {
		/* a k�d, amellyel rajzolni tudunk a GLFWwindow ojektumunkba. */
		display(window, glfwGetTime());
		/* double buffered */
		glfwSwapBuffers(window);
		/* esem�nyek kezel�se az ablakunkkal kapcsolatban, pl. gombnyom�s */
		glfwPollEvents();
	}

	cleanUpScene();
	/* t�r�lj�k a GLFW ablakot. */
	glfwDestroyWindow(window);
	/* Le�ll�tjuk a GLFW-t */
	glfwTerminate();

	exit(EXIT_SUCCESS);
}