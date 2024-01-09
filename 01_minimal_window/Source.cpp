#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

extern void cleanUpScene();

GLFWwindow		*window;

// normal keys are fom [0..255], arrow and special keys are from [256..511]
GLboolean		keyboard[512] = {GL_FALSE};

int				window_width = 600;
int				window_height = 600;
char			window_title[] = "Lighting and Z buffer";

unsigned int	modelLoc;
unsigned int	viewLoc;
unsigned int	projectionLoc;
unsigned int	invTMatrixLoc;
unsigned int	lightPosLoc;

float vertices[] = {
		// positions          normals
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
};



float vertices1[] = {
	// positions          normals
	-4.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 -3.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 -3.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 -3.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-4.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-4.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-4.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 -3.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 -3.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 -3.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-4.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-4.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-4.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
	-4.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
	-4.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
	-4.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
	-4.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
	-4.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

	 -3.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 -3.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 -3.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 -3.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 -3.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 -3.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-4.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 -3.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 -3.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 -3.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-4.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-4.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-4.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 -3.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 -3.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 -3.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-4.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-4.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
};


float vertices2[] = {
	// positions          normals
	5.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 6.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 6.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 6.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	5.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	5.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	5.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 6.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 6.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 6.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	5.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	5.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	5.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
	5.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
	5.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
	5.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
	5.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
	5.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

	 6.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 6.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 6.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 6.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 6.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 6.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	5.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	6.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 6.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 6.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	5.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	5.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	5.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 6.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 6.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 6.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	5.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	5.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
};

/* Vertex buffer objektum �s vertex array objektum az adatt�rol�shoz.*/
#define numVBOs	3
#define numVAOs	3
GLuint			VBO[numVBOs];
GLuint			VAO[numVAOs];

GLuint		renderingProgram;

/** Modell, vet�t�si �s kamera m�trixok felv�tele. */
glm::mat4	model,model2,model3, view,view2, projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
glm::mat4	invTmatrix,invTmatrix2, rotateM, scaleM,scaleM2;

GLdouble	currentTime, deltaTime, lastTime = 0.0f;
GLfloat		cameraSpeed,valtoz = 0.0f;
GLboolean   boing = true;

/* Vegy�k fel a kamera pozic�t t�rol� v�ltoz�t, illetve a tengelyekhezz sz�ks�ges vektorokat. */
glm::vec3	cameraPos		= glm::vec3( 2.0f, 2.0f, 5.0f),
			cameraTarget	= glm::vec3( 0.0f, 0.0f, 0.0f),
			cameraUpVector	= glm::vec3( 0.0f, 1.0f, 0.0f),
			cameraMovingX	= glm::vec3(-1.0f, 0.0f, 0.0f),
			cameraMovingY	= glm::vec3( 0.0f, 1.0f, 0.0f),
			cameraMovingZ   = glm::vec3(0.0f, 0.0f, 1.0f);
// it is a headlight, it will follow the camera
glm::vec3	lightPos;

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

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		cleanUpScene();

	if (action == GLFW_PRESS)
		keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE)
		keyboard[key] = GL_FALSE;
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
}

void computeModelMatrix() {
//	rotateM	= glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleM	= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));


	model = scaleM;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void computeModelMatrix2() {

	//rotateM	= glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleM = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));


	model2 = scaleM * rotateM;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
}
void computeModelMatrix3() {

	//rotateM	= glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	model3 = scaleM2 * rotateM;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
}

/** Kisz�moljuk a kamera m�trixokat. */
void computeCameraMatrix() {
	/* A param�terek rendre: az �j koordin�tarendszer�nk k�z�ppontja (hol a kamera), merre n�z a kamera, mit tekint�nk ,,f�lfele" ir�nynak */
	view	= glm::lookAt(cameraPos, cameraTarget, cameraUpVector);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}
void computeCameraMatrix2() {
	/* A param�terek rendre: az �j koordin�tarendszer�nk k�z�ppontja (hol a kamera), merre n�z a kamera, mit tekint�nk ,,f�lfele" ir�nynak */
	view2 = glm::lookAt(cameraPos, cameraTarget, cameraUpVector);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view2));
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/* Csatoljuk a vertex array objektumunkat a konfigur�l�shoz. */
	glBindVertexArray(VAO[0]);

	/* Ezen adatok szolg�lj�k a 0 index� vertex attrib�tumot (itt: poz�ci�).
	Els�k�nt megadjuk ezt az azonos�t�sz�mot.
	Ut�na az attrib�tum m�ret�t (vec3, l�ttuk a shaderben).
	Harmadik az adat t�pusa.
	Negyedik az adat normaliz�l�sa, ez maradhat FALSE jelen p�ld�ban.
	Az attrib�tum �rt�kek hogyan k�vetkeznek egym�s ut�n? Milyen l�p�sk�z ut�n tal�lom a k�vetkez� vertex adatait?
	V�g�l megadom azt, hogy honnan kezd�dnek az �rt�kek a pufferben. Most r�gt�n, a legelej�t�l veszem �ket.*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	/* Enged�lyezz�k az im�nt defini�lt 0 index� attrib�tumot. */
	glEnableVertexAttribArray(0);

	/* Norm�lvektor attrib�tum */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* Lev�lasztjuk a vertex array objektumot �s a puufert is.*/
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);





	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

	/* M�soljuk az adatokat a pufferbe! Megadjuk az aktu�lisan csatolt puffert,  azt hogy h�ny b�jt adatot m�solunk,
	a m�soland� adatot, majd a feldolgoz�s m�dj�t is meghat�rozzuk: most az adat nem v�ltozik a felt�lt�s ut�n */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glBindVertexArray(VAO[1]);

	/* Ezen adatok szolg�lj�k a 0 index� vertex attrib�tumot (itt: poz�ci�).
	Els�k�nt megadjuk ezt az azonos�t�sz�mot.
	Ut�na az attrib�tum m�ret�t (vec3, l�ttuk a shaderben).
	Harmadik az adat t�pusa.
	Negyedik az adat normaliz�l�sa, ez maradhat FALSE jelen p�ld�ban.
	Az attrib�tum �rt�kek hogyan k�vetkeznek egym�s ut�n? Milyen l�p�sk�z ut�n tal�lom a k�vetkez� vertex adatait?
	V�g�l megadom azt, hogy honnan kezd�dnek az �rt�kek a pufferben. Most r�gt�n, a legelej�t�l veszem �ket.*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	/* Enged�lyezz�k az im�nt defini�lt 0 index� attrib�tumot. */
	glEnableVertexAttribArray(0);

	/* Norm�lvektor attrib�tum */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* Lev�lasztjuk a vertex array objektumot �s a puufert is.*/
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);













	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);

	/* M�soljuk az adatokat a pufferbe! Megadjuk az aktu�lisan csatolt puffert,  azt hogy h�ny b�jt adatot m�solunk,
	a m�soland� adatot, majd a feldolgoz�s m�dj�t is meghat�rozzuk: most az adat nem v�ltozik a felt�lt�s ut�n */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glBindVertexArray(VAO[2]);

	/* Ezen adatok szolg�lj�k a 0 index� vertex attrib�tumot (itt: poz�ci�).
	Els�k�nt megadjuk ezt az azonos�t�sz�mot.
	Ut�na az attrib�tum m�ret�t (vec3, l�ttuk a shaderben).
	Harmadik az adat t�pusa.
	Negyedik az adat normaliz�l�sa, ez maradhat FALSE jelen p�ld�ban.
	Az attrib�tum �rt�kek hogyan k�vetkeznek egym�s ut�n? Milyen l�p�sk�z ut�n tal�lom a k�vetkez� vertex adatait?
	V�g�l megadom azt, hogy honnan kezd�dnek az �rt�kek a pufferben. Most r�gt�n, a legelej�t�l veszem �ket.*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	/* Enged�lyezz�k az im�nt defini�lt 0 index� attrib�tumot. */
	glEnableVertexAttribArray(0);

	/* Norm�lvektor attrib�tum */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* Lev�lasztjuk a vertex array objektumot �s a puufert is.*/
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);






	/** Aktiv�ljuk a shader-program objektumunkat. */
	glUseProgram(renderingProgram);

	modelLoc		= glGetUniformLocation(renderingProgram, "model");
	viewLoc			= glGetUniformLocation(renderingProgram, "view");
	projectionLoc	= glGetUniformLocation(renderingProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	invTMatrixLoc	= glGetUniformLocation(renderingProgram, "invTMatrix");
	lightPosLoc		= glGetUniformLocation(renderingProgram, "lightPos");

	// background is black
	glClearColor(0.0, 0.0, 0.0, 1.0);

	/** Z-bufferez�s enged�lyez�se. */
	// enable Z coordinate check for visibility
	glEnable(GL_DEPTH_TEST);
}

/** A jelenet�nk ut�ni takar�t�s. */
void cleanUpScene() {
	/* t�r�lj�k a GLFW ablakot. */
	glfwDestroyWindow(window);

	/** T�r�lj�k a vertex puffer �s vertex array objektumokat. */
	glDeleteVertexArrays(numVAOs, VAO);
	glDeleteBuffers(numVBOs, VBO);

	/** T�r�lj�k a shader programot. */
	glDeleteProgram(renderingProgram);

	/* Le�ll�tjuk a GLFW-t */
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	cameraSpeed = 5.0f * (GLfloat)deltaTime;


	/* A kamera sz�m�ra sz�m�tsuk ki az el�re mutat� ir�nyt .*/
	if ((keyboard[GLFW_KEY_W]) || (keyboard[GLFW_KEY_UP]))
		cameraPos += cameraSpeed * cameraMovingY;

	if ((keyboard[GLFW_KEY_S]) || (keyboard[GLFW_KEY_DOWN]))
		cameraPos -= cameraSpeed * cameraMovingY;

	if ((keyboard[GLFW_KEY_A]) || (keyboard[GLFW_KEY_LEFT]))
		cameraPos += cameraSpeed * cameraMovingX;

	if ((keyboard[GLFW_KEY_D]) || (keyboard[GLFW_KEY_RIGHT]))
		cameraPos -= cameraSpeed * cameraMovingX;

	if ((keyboard[GLFW_KEY_Q]))
		cameraPos += cameraSpeed * cameraMovingZ;

	if ((keyboard[GLFW_KEY_E]))
		cameraPos -= cameraSpeed * cameraMovingZ;

	// light follow the user viewing the scene, behaves as a headlamp

	rotateM = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	computeModelMatrix();
	computeCameraMatrix();

	invTmatrix = glm::inverseTranspose(view * model);
	glUniformMatrix4fv(invTMatrixLoc, 1, GL_FALSE, glm::value_ptr(invTmatrix));

	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

	/*Csatoljuk a vertex array objektumunkat. */
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));
	/* Lev�lasztjuk, nehogy b�rmilyen �rt�k fel�l�r�djon.*/
	glBindVertexArray(0);



	computeModelMatrix2();
	computeCameraMatrix2();

	invTmatrix2 = glm::inverseTranspose(view2 * model2);
	glUniformMatrix4fv(invTMatrixLoc, 1, GL_FALSE, glm::value_ptr(invTmatrix2));


	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices1));
	/* Lev�lasztjuk, nehogy b�rmilyen �rt�k fel�l�r�djon.*/
	glBindVertexArray(0);


	rotateM = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	if (boing==true)
	{
		scaleM2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, valtoz, 0.5f));
		valtoz = valtoz + 0.01f;
		if (valtoz > 2.0f)
			boing = false;
	}
	else
	{
		scaleM2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, valtoz, 0.5f));
		valtoz = valtoz - 0.01f;
		if (valtoz < 0.2f)
			boing = true;
	}

	computeModelMatrix3();
	computeCameraMatrix2();

	invTmatrix2 = glm::inverseTranspose(view2 * model3);
	glUniformMatrix4fv(invTMatrixLoc, 1, GL_FALSE, glm::value_ptr(invTmatrix2));

	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices2));
	/* Lev�lasztjuk, nehogy b�rmilyen �rt�k fel�l�r�djon.*/
	glBindVertexArray(0);

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	window_width	= width;
	window_height	= height;

	glViewport(0, 0, width, height);

	projection		= glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

int main(void) {
	/* Pr�b�ljuk meg inicializ�lni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/* A k�v�nt OpenGL verzi� (4.3) */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/* Pr�b�ljuk meg l�trehozni az ablakunkat. */
	window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);

	/* V�lasszuk ki az ablakunk OpenGL kontextus�t, hogy haszn�lhassuk. */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/* Incializ�ljuk a GLEW-t, hogy el�rhet�v� v�ljanak az OpenGL f�ggv�nyek. */
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	/* Az alkalmaz�shoz kapcsol�d� el�k�sz�t� l�p�sek, pl. hozd l�tre a shader objektumokat. */
	init(window);

	while (!glfwWindowShouldClose(window)) {
		/* a k�d, amellyel rajzolni tudunk a GLFWwindow ojektumunkba. */
		display();
		/* double buffered */
		glfwSwapBuffers(window);
		/* esem�nyek kezel�se az ablakunkkal kapcsolatban, pl. gombnyom�s */
		glfwPollEvents();
	}

	cleanUpScene();

	return EXIT_SUCCESS;
}