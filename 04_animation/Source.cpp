#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
/* Szükséges az M_PI használatához. */
/* Needed for using M_PI. */
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <vector>

using namespace std;

/** Hagyományos tömb is használható a csúcspontok tárolásához. */
/** C array can be used to store vertices also. */
std::vector<glm::vec3> linePoints;
std::vector<glm::vec3> circlePoints;
std::vector<glm::vec3> circleColorPoints;
static glm::vec2 circleCenter = glm::vec2(0.0f, 0.0f);

/* Vertex buffer objektum és vertex array objektum az adattároláshoz. */
/* Vertex buffer object ID and vertex array object ID for data storing.*/
#define		numVBOs			3
#define		numVAOs			2
GLuint		VBO[numVBOs];
GLuint		VAO[numVAOs];

int			window_width = 600;
int			window_height = 600;
char		window_title[] = "Moving triangle";
/** A normál billentyûk a [0..255] tartományban vannak, a nyilak és a speciális billentyûk pedig a [256..511] tartományban helyezkednek el. */
/** Normal keys are fom [0..255], arrow and special keys are from [256..511]. */
GLboolean	keyboard[512] = { GL_FALSE };
GLFWwindow* window = nullptr;
GLuint		renderingProgram;
GLuint		XoffsetLocation;
GLuint		centerPoint;
GLuint		symmetryLocation;
GLboolean	isLine = false;
float		symmY = 0.00f;
float		x = 0.00f;
float		incrementx = 0.005f;

bool		xDir = true;
bool		yDir = true;

bool checkOpenGLError() {
	bool	foundError = false;
	int		glErr = glGetError();

	/** Vizsgáljuk meg, hogy van-e aktuálisan OpenGL hiba, és amennyiben igen, írassuk ki azokat a konzolra egyenként. */
	/** Check for OpenGL errors, and send them to the console ony by one. */
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;

		foundError = true;
		glErr = glGetError();
	}

	/** Ha van aktuálisan OpenGL hiba, a visszatérési érték true. */
	/** If there are OpenGL errors, the return value is true. */
	return foundError;
}

void printShaderLog(GLuint shader) {
	int		length = 0;
	int		charsWritten = 0;
	char* log = nullptr;

	/** Vizsgáljuk meg, hogy van-e valami a Shader Info Logban, és amennyiben igen, írassuk ki azt a konzolra soronként. */
	/** Check for Shader Info Log, and send it to the console by lines if it is created for the last compile. */
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

	if (length > 0) {
		log = (char*)malloc(length);

		/** Olvassuk és írassuk ki a következõ sort. */
		/** Read out and and send to the console the next line. */
		glGetShaderInfoLog(shader, length, &charsWritten, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int		length = 0;
	int		charsWritten = 0;
	char* log = nullptr;

	/** Vizsgáljuk meg, hogy van-e valami a Program Info Logban, és amennyiben igen, írassuk ki azt a konzolra soronként. */
	/** Check for Program Info Log, and send it to the console by lines if it is created for the last compile. */
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length);

	if (length > 0) {
		log = (char*)malloc(length);

		/** Olvassuk és írassuk ki a következõ sort. */
		/** Read out and and send to the console the next line. */
		glGetProgramInfoLog(prog, length, &charsWritten, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

string readShaderSource(const char* filePath) {
	/** A file stream inicializálása olvasásra. */
	/** Let's initialize the file stream for reading. */
	ifstream	fileStream(filePath, ios::in);
	string		content;
	string		line;

	/** A shader fájl sorainak beolvasása EOF jelzésig. (EOF = End Of File) */
	/** Read in the lines of the shader file until EOF. (EOF = End Of File) */
	while (!fileStream.eof()) {
		getline(fileStream, line);
		/** A shader fájl sorainak összefûzése. */
		/** Append the lines of the shader file. */
		content.append(line + "\n");
	}

	/** A file stream lezárása. */
	/** Let's close the file stream. */
	fileStream.close();

	/** Visszatérés a shader fájl tartalmával. */
	/** Return the content of the shader file. */
	return content;
}

GLuint createShaderProgram() {
	GLint		vertCompiled;
	GLint		fragCompiled;
	GLint		linked;
	/** Beolvassuk a shader fájlok tartalmát. */
	/** Read in both shader files. */
	string		vertShaderStr = readShaderSource("vertexShader.glsl");
	string		fragShaderStr = readShaderSource("fragmentShader.glsl");
	/** Létrehozzuk a shader objektumokat és eltároljuk az ID-ket. */
	/** Let's create the shader objects and store the IDs. */
	GLuint		vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint		fShader = glCreateShader(GL_FRAGMENT_SHADER);
	/** A shader fájlok tartalmát eltároló string objektum szöveggé konvertálásás is elvégezzük. */
	/** The contents of the shader string objects shall be converted to text of characters. */
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	/** Ekkor a betöltött kódot hozzárendelhetjük a shader objektumainkhoz. */
	/** The loaded source codes are assigned to the shader objects. */
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	/** Fordítsuk le a vertex shader objektumhoz rendelt kódot. */
	/** Let's compile the code of the vertex shader object. */
	glCompileShader(vShader);
	/** Hibakeresési lépések. Például sikeres volt-e a fordítás? Ha nem, mi volt az oka? */
	/** Error checking. Was the compile step successful? If not, what was the reason? */
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "Vertex compilation failed." << endl;
		printShaderLog(vShader);
	}

	/** Fordítsuk le a fragment shader objektumhoz rendelt kódot. */
	/** Let's compile the code of the fragment shader object. */
	glCompileShader(fShader);
	/** Hibakeresési lépések. Például sikeres volt-e a fordítás? Ha nem, mi volt az oka? */
	/** Error checking. Was the compile step successful? If not, what was the reason? */
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "Fragment compilation failed." << endl;
		printShaderLog(fShader);
	}

	/** Shader program objektum létrehozása: ez fogja össze a shadereket. Eltároljuk az ID értéket. */
	/** Shader program object initialization: holds together the shaders. vfProgram stores the ID. */
	GLuint		vfProgram = glCreateProgram();
	/** Csatoljuk a shadereket az elõzõ lépésben létrehozott objektumhoz. */
	/** The shaders are attached to the program object. */
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	/** Végül a GLSL szerkesztõ ellenõrzi, hogy a csatolt shaderek kompatibilisek-e egymással. */
	/** GLSL linker checks the shaders for compatibility. */
	glLinkProgram(vfProgram);
	/** Hibakeresési lépések. Például sikeres volt-e az összeszerkesztés? Ha nem, mi volt az oka? */
	/** Error checking. Was the link step successful? If not, what was the reason? */
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "Shader linking failed." << endl;
		printProgramLog(vfProgram);
	}

	/** Ha minden rendben ment a linkelés során, a shader objektumok törölhetõek. */
	/** If everything is OK at linking, the shader objects can be destroyed. */
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	/** Az elkészült program azonosítója a visszatérési értékünk. */
	/** The program ID will be the return value. */
	return vfProgram;
}
void generateCirclePoints(glm::vec2 center_point, GLfloat a, GLfloat b, GLint num_segment) {
	GLfloat		x, y;
	GLfloat		alpha = 0.0f;
	GLfloat		full_circle = 2.0f * M_PI;
	GLfloat		delta = full_circle / num_segment;
	x = center_point.x + a * cos(alpha);
	y = center_point.y + b * sin(alpha);
	circlePoints.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	circleColorPoints.push_back(glm::vec3(0.7, 0.1, 0.1));

	for (int i = 0; i <= num_segment; i++)
	{
		x = center_point.x + a * cos(alpha);
		y = center_point.y + b * sin(alpha);
		circlePoints.push_back(glm::vec3(x, y, 0.0f));
		circleColorPoints.push_back(glm::vec3(0.1, 0.95, 0.1));
		alpha += delta;
	}
	circlePoints.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	circleColorPoints.push_back(glm::vec3(0.6, 0.1, 0.1));
}
void generateLinePoints(glm::vec2 start_point, glm::vec2 vector, GLint num_segment) {
	GLfloat x, y;
	GLfloat t = 0.0f;

	for (int i = 0; i <= num_segment; i++) {
		x = start_point.x + t * vector.x;
		y = start_point.y + t * vector.y;

		linePoints.push_back(glm::vec3(x, y, 0.0f));

		t += 1.0f / num_segment;
	}
}




void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram();

	generateLinePoints(glm::vec2(-0.33f, -0.0f), glm::vec2(0.66f, 0.00f), 4);
	generateCirclePoints(circleCenter, 0.25f, 0.25f, 120);

	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	generateCirclePoints(glm::vec2(0.0f, 0.0f), 0.25f, 0.25f, 120);
	glBindVertexArray(VAO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, circlePoints.size() * sizeof(glm::vec3), circlePoints.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, circleColorPoints.size() * sizeof(glm::vec3), circleColorPoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, linePoints.size() * sizeof(glm::vec3), linePoints.data(), GL_STATIC_DRAW);
	glBindVertexArray(VAO[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	XoffsetLocation = glGetUniformLocation(renderingProgram, "offsetX");
	symmetryLocation = glGetUniformLocation(renderingProgram, "symmetryLocation"); 
	centerPoint = glGetUniformLocation(renderingProgram, "centerPoint");

	glUseProgram(renderingProgram);
	glClearColor(0.9, 0.8, 0.2, 1.0);

}

void display(GLFWwindow* window, double currentTime) {
	glLineWidth(3.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	if (xDir) {
		x += incrementx; // mozgassuk a háromszöget az x-tengely mentén.
		if (x > 0.75f) incrementx = incrementx * -1; // váltsunk, balra mozogjon a háromszög.
		if (x < -0.75f) incrementx = incrementx * -1; // váltsunk, jobbra mozogjon a háromszög.
		glProgramUniform1f(renderingProgram, XoffsetLocation, x); // küldjük el az "x" értékét az "offsetX" számára
	}
	glUniform1i(glGetUniformLocation(renderingProgram, "isLine"), 1);
	glBindVertexArray(VAO[1]);
	glLineWidth(3.0);
	glDrawArrays(GL_LINE_LOOP, 0, 5);
	glBindVertexArray(0);
	glUniform1i(glGetUniformLocation(renderingProgram, "isLine"), 0);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, circlePoints.size());
	glBindVertexArray(0);


}









/** Felesleges objektumok törlése. */
/** Clenup the unnecessary objects. */
void cleanUpScene() {
	/** Töröljük a shader programot. */
	/** Let's delete the shader program. */
	glDeleteProgram(renderingProgram);
	/** Töröljük a GLFW ablakot. */
	/** Destroy the GLFW window. */
	glfwDestroyWindow(window);
	/** Leállítjuk a GLFW-t. */
	/** Stop the GLFW system. */
	glfwTerminate();
	/** Kilépés EXIT_SUCCESS kóddal. */
	/** Stop the software and exit with EXIT_SUCCESS code. */
	exit(EXIT_SUCCESS);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	window_width = width;
	window_height = height;

	/** A kezelt képernyõ beállítása a teljes (0, 0, width, height) területre. */
	/** Set the viewport for the full (0, 0, width, height) area. */
	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/** ESC billentyûre kilépés. */
	/** Exit on ESC key. */
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		cleanUpScene();

	/** A billentyûk lenyomásának és felengedésének regisztrálása. Lehetõvé teszi gombkombinációk használatát. */
	/** Let's register press and release events for keys. Enables the usage of key combinations. */
	if (action == GLFW_PRESS)
		keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE)
		keyboard[key] = GL_FALSE;

	// Up arrow key for moving symmetry axis up
	if (key == GLFW_KEY_UP && action == GLFW_PRESS && symmY <= 1.00f) {
		symmY += 0.05f;
		glProgramUniform1f(renderingProgram, symmetryLocation, symmY);
	}
	// Down arrow key for moving symmetry axis down
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && symmY >= -1.00f) {
		symmY -= 0.05f;
		glProgramUniform1f(renderingProgram, symmetryLocation, symmY);
	}
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	/** Az egér mutató helyét kezelõ függvény. */
	/** Callback function for mouse position change. */
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	/** Az egér gombjaihoz köthetõ események kezelése. */
	/** Callback function for mouse button events. */
}

int main(void) {
	/** Próbáljuk meg inicializálni a GLFW-t! */
	/** Try to initialize GLFW! */
	if (!glfwInit())
		exit(EXIT_FAILURE);

	/** A használni kívánt OpenGL verzió: 4.3. */
	/** The needed OpenGL version: 4.3. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed.
	/** Próbáljuk meg létrehozni az ablakunkat. */
	/** Let's try to create a window for drawing. */
	/** GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor * monitor, GLFWwindow * share) */
	window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);

	/** Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
	/** Select the OpenGL context (window) for drawing. */
	glfwMakeContextCurrent(window);

	/** A képernyõ átméretezés kezelése. */
	/** Callback function for window size change. */
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	/** Billentyûzethez köthetõ események kezelése. */
	/** Callback function for keyboard events. */
	glfwSetKeyCallback(window, keyCallback);
	/** Az egér mutató helyét kezelõ függvény megadása. */
	/** Callback function for mouse position change. */
	glfwSetCursorPosCallback(window, cursorPosCallback);
	/** Az egér gombjaihoz köthetõ események kezelése. */
	/** Callback function for mouse button events. */
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/** Incializáljuk a GLEW-t, hogy elérhetõvé váljanak az OpenGL függvények, probléma esetén kilépés EXIT_FAILURE értékkel. */
	/** Initalize GLEW, so the OpenGL functions will be available, on problem exit with EXIT_FAILURE code. */
	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	/** 0 = v-sync kikapcsolva, 1 = v-sync bekapcsolva, n = n db képkockányi idõt várakozunk */
	/** 0 = v-sync off, 1 = v-sync on, n = n pieces frame time waiting */
	glfwSwapInterval(1);

	/** A window ablak minimum és maximum szélességének és magasságának beállítása. */
	/** The minimum and maximum width and height values of the window object. */
	glfwSetWindowSizeLimits(window, 400, 400, 800, 800);
	/** A window oldalarányának megadása a számláló és az osztó segítségével. */
	/** Setting the aspect ratio using the numerator and the denominator values. */
	glfwSetWindowAspectRatio(window, 1, 1);

	/** Az alkalmazáshoz kapcsolódó elõkészítõ lépések, pl. a shader objektumok létrehozása. */
	/** The first initialization steps of the program, e.g.: creating the shader objects. */
	init(window);

	/** A megadott window struktúra "close flag" vizsgálata. */
	/** Checks the "close flag" of the specified window. */
	while (!glfwWindowShouldClose(window)) {
		/** A kód, amellyel rajzolni tudunk a GLFWwindow objektumunkba. */
		/** Call display function which will draw into the GLFWwindow object. */
		display(window, glfwGetTime());
		/** Double buffered mûködés. */
		/** Double buffered working = swap the front and back buffer here. */
		glfwSwapBuffers(window);
		/** Események kezelése az ablakunkkal kapcsolatban, pl. gombnyomás. */
		/** Handle events related to our window, e.g.: pressing a key or moving the mouse. */
		glfwPollEvents();
	}

	/** Felesleges objektumok törlése. */
	/** Clenup the unnecessary objects. */
	cleanUpScene();

	/** Kilépés EXIT_SUCCESS kóddal. */
	/** Stop the software and exit with EXIT_SUCCESS code. */
	return EXIT_SUCCESS;
}