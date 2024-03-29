#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

using namespace std;

/** std::array haszn�lhat� a cs�cspontok t�rol�s�hoz. */
/** std::array can be used to store vertices. */
/*
static std::array<glm::vec3, 3> vertices =
{
	glm::vec3(-0.5f, -0.5f, 0.0f),
	glm::vec3( 0.5f, -0.5f, 0.0f),
	glm::vec3( 0.0f,  0.5f, 0.0f),
};
*/

/** Hagyom�nyos t�mb is haszn�lhat� a cs�cspontok t�rol�s�hoz. */
/** C array can be used to store vertices also. */
static GLfloat s_vertices[] =
{
	0.0f,0.0f,0.0f,
	 0.0f,  -0.5f, 0.0f,
	-0.5f, -0.3f, 0.0f,
	 -0.5f, 0.3f, 0.0f,
	 0.0f,  0.5f, 0.0f ,
	  0.5f, 0.3f, 0.0f,
	 0.5f, -0.3f, 0.0f,
	 0.0f,  -0.5f, 0.0f,
};

/** Hagyom�nyos t�mb haszn�lhat� a sz�nek t�rol�s�hoz. */
/** C array can be used to store colors. */
static GLfloat s_colors[] =
{
	0.0f,0.0f,0.0f,
	1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.0f,
	0.0f, 1.0f, 1.0f,	
	0.0f, 0.5f, 0.5f,
	0.0f, 0.0f, 1.0f,
	0.5f, 0.0f, 0.5f,
	1.0f, 0.0f, 0.0f
};

/* Vertex buffer objektumok �s vertex array objektum az adatt�rol�shoz. */
/* Vertex buffer object IDs and vertex array object ID for data storing.*/
#define		numVBOs			2
#define		numVAOs			1
GLuint		VBO[numVBOs];
GLuint		VAO[numVAOs];

int			window_width	= 600;
int			window_height	= 600;
char		window_title[]	= "VAO and VBO";
/** A norm�l billenty�k a [0..255] tartom�nyban vannak, a nyilak �s a speci�lis billenty�k pedig a [256..511] tartom�nyban helyezkednek el. */
/** Normal keys are fom [0..255], arrow and special keys are from [256..511]. */
GLboolean	keyboard[512]	= {GL_FALSE};
GLFWwindow	*window			= nullptr;
GLuint		renderingProgram;


bool checkOpenGLError() {
	bool	foundError	= false;
	int		glErr		= glGetError();

	/** Vizsg�ljuk meg, hogy van-e aktu�lisan OpenGL hiba, �s amennyiben igen, �rassuk ki azokat a konzolra egyenk�nt. */
	/** Check for OpenGL errors, and send them to the console ony by one. */
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;

		foundError	= true;
		glErr		= glGetError();
	}

	/** Ha van aktu�lisan OpenGL hiba, a visszat�r�si �rt�k true. */
	/** If there are OpenGL errors, the return value is true. */
	return foundError;
}

void printShaderLog(GLuint shader) {
	int		length			= 0;
	int		charsWritten	= 0;
	char	*log			= nullptr;
	
	/** Vizsg�ljuk meg, hogy van-e valami a Shader Info Logban, �s amennyiben igen, �rassuk ki azt a konzolra soronk�nt. */
	/** Check for Shader Info Log, and send it to the console by lines if it is created for the last compile. */
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	
	if (length > 0) {
		log = (char*)malloc(length);

		/** Olvassuk �s �rassuk ki a k�vetkez� sort. */
		/** Read out and and send to the console the next line. */
		glGetShaderInfoLog(shader, length, &charsWritten, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int		length			= 0;
	int		charsWritten	= 0;
	char	*log			= nullptr;
	
	/** Vizsg�ljuk meg, hogy van-e valami a Program Info Logban, �s amennyiben igen, �rassuk ki azt a konzolra soronk�nt. */
	/** Check for Program Info Log, and send it to the console by lines if it is created for the last compile. */
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length);
	
	if (length > 0) {
		log = (char*)malloc(length);

		/** Olvassuk �s �rassuk ki a k�vetkez� sort. */
		/** Read out and and send to the console the next line. */
		glGetProgramInfoLog(prog, length, &charsWritten, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

string readShaderSource(const char* filePath) {
	/** A file stream inicializ�l�sa olvas�sra. */
	/** Let's initialize the file stream for reading. */
	ifstream	fileStream(filePath, ios::in);
	string		content;
	string		line;

	/** A shader f�jl sorainak beolvas�sa EOF jelz�sig. (EOF = End Of File) */
	/** Read in the lines of the shader file until EOF. (EOF = End Of File) */
	while (!fileStream.eof()) {
		getline(fileStream, line);
		/** A shader f�jl sorainak �sszef�z�se. */
		/** Append the lines of the shader file. */
		content.append(line + "\n");
	}

	/** A file stream lez�r�sa. */
	/** Let's close the file stream. */
	fileStream.close();
	
	/** Visszat�r�s a shader f�jl tartalm�val. */
	/** Return the content of the shader file. */
	return content;
}

GLuint createShaderProgram() {
	GLint		vertCompiled;
	GLint		fragCompiled;
	GLint		linked;
	/** Beolvassuk a shader f�jlok tartalm�t. */
	/** Read in both shader files. */
	string		vertShaderStr = readShaderSource("vertexShader.glsl");
	string		fragShaderStr = readShaderSource("fragmentShader.glsl");
	/** L�trehozzuk a shader objektumokat �s elt�roljuk az ID-ket. */
	/** Let's create the shader objects and store the IDs. */
	GLuint		vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint		fShader = glCreateShader(GL_FRAGMENT_SHADER);
	/** A shader f�jlok tartalm�t elt�rol� string objektum sz�vegg� konvert�l�s�s is elv�gezz�k. */
	/** The contents of the shader string objects shall be converted to text of characters. */
	const char	*vertShaderSrc = vertShaderStr.c_str();
	const char	*fragShaderSrc = fragShaderStr.c_str();

	/** Ekkor a bet�lt�tt k�dot hozz�rendelhetj�k a shader objektumainkhoz. */
	/** The loaded source codes are assigned to the shader objects. */
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	/** Ford�tsuk le a vertex shader objektumhoz rendelt k�dot. */
	/** Let's compile the code of the vertex shader object. */
	glCompileShader(vShader);
	/** Hibakeres�si l�p�sek. P�ld�ul sikeres volt-e a ford�t�s? Ha nem, mi volt az oka? */
	/** Error checking. Was the compile step successful? If not, what was the reason? */
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "Vertex compilation failed." << endl;
		printShaderLog(vShader);
	}

	/** Ford�tsuk le a fragment shader objektumhoz rendelt k�dot. */
	/** Let's compile the code of the fragment shader object. */
	glCompileShader(fShader);
	/** Hibakeres�si l�p�sek. P�ld�ul sikeres volt-e a ford�t�s? Ha nem, mi volt az oka? */
	/** Error checking. Was the compile step successful? If not, what was the reason? */
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "Fragment compilation failed." << endl;
		printShaderLog(fShader);
	}

	/** Shader program objektum l�trehoz�sa: ez fogja �ssze a shadereket. Elt�roljuk az ID �rt�ket. */
	/** Shader program object initialization: holds together the shaders. vfProgram stores the ID. */
	GLuint		vfProgram = glCreateProgram();
	/** Csatoljuk a shadereket az el�z� l�p�sben l�trehozott objektumhoz. */
	/** The shaders are attached to the program object. */
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	/** V�g�l a GLSL szerkeszt� ellen�rzi, hogy a csatolt shaderek kompatibilisek-e egym�ssal. */
	/** GLSL linker checks the shaders for compatibility. */
	glLinkProgram(vfProgram);
	/** Hibakeres�si l�p�sek. P�ld�ul sikeres volt-e az �sszeszerkeszt�s? Ha nem, mi volt az oka? */
	/** Error checking. Was the link step successful? If not, what was the reason? */
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "Shader linking failed." << endl;
		printProgramLog(vfProgram);
	}

	/** Ha minden rendben ment a linkel�s sor�n, a shader objektumok t�r�lhet�ek. */
	/** If everything is OK at linking, the shader objects can be destroyed. */
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	/** Az elk�sz�lt program azonos�t�ja a visszat�r�si �rt�k�nk. */
	/** The program ID will be the return value. */
	return vfProgram;
}

void init(GLFWwindow* window) {
	/** A rajzol�shoz haszn�lt shader programok bet�lt�se. */
	/** Loading the shader programs for rendering. */
	renderingProgram = createShaderProgram();

	/* L�trehozzuk a sz�ks�ges vertex buffer �s vertex array objektumokat. */
	/* Create the vertex buffer and vertex array objects. */
	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);

	/* A GL_ARRAY_BUFFER neves�tett csatol�ponthoz kapcsoljuk a vertex buffert (ide ker�lnek a cs�cspont adatok). */
	/* We attach the vertex buffer to the GL_ARRAY_BUFFER node (vertices are stored here). */
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	/* M�soljuk az adatokat a bufferbe! Megadjuk az aktu�lisan csatolt buffert, azt hogy h�ny byte adatot m�solunk,
	a m�soland� adatot, majd a feldolgoz�s m�dj�t is meghat�rozzuk: most az adat nem v�ltozik a felt�lt�s ut�n. */
	/* Copy the data to the buffer! First parameter is the currently attached buffer, second is the size of the buffer to be copied,
	third is the array of data, fourth is working mode: now the data can not be modified after this step. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);
	/* std::array megad�sa param�ternek. */
	/* std::array as parameter. */
	// glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	/* Csatoljuk a vertex array objektumunkat a shader programhoz. */
	/* Attach the vertex array object to the shader program. */
	glBindVertexArray(VAO[0]);
	/* Ezen adatok szolg�lj�k a location = 0 vertex attrib�tumot (itt: poz�ci�).
	   Els�k�nt megadjuk ezt az azonos�t�sz�mot (vertexShader.glsl).
	   Ut�na az attrib�tum m�ret�t (vec3, l�ttuk a shaderben).
	   Harmadik az adat t�pusa.
	   Negyedik az adat normaliz�l�sa, ez maradhat FALSE jelen p�ld�ban.
	   Az attrib�tum �rt�kek hogyan k�vetkeznek egym�s ut�n? Milyen l�p�sk�z ut�n tal�lom a k�vetkez� vertex adatait?
	   V�g�l megadom azt, hogy honnan kezd�dnek az �rt�kek a pufferben. Most r�gt�n, a legelej�t�l veszem �ket. */
	/* These values are for location = 0 vertex attribute (position).
	   First is the location (vertexShader.glsl).
	   Second is attribute size (vec3, as in the shader).
	   Third is the data type.
	   Fourth defines whether data shall be normalized or not, this is FALSE for the examples of the course.
	   Fifth is the distance in bytes to the next vertex element of the array.
	   Last is the offset of the first vertex data of the buffer. Now it is the start of the array. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); 
	/* Enged�lyezz�k az im�nt defini�lt location = 0 attrib�tumot (vertexShader.glsl). */
	/* Enable the previously defined location = 0 attributum (vertexShader.glsl). */
	glEnableVertexAttribArray(0);

	/* A GL_ARRAY_BUFFER neves�tett csatol�ponthoz kapcsoljuk a color buffert (ide ker�lnek a sz�n adatok). */
	/* We attach the color buffer to the GL_ARRAY_BUFFER node (color values are stored here). */
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	/* M�soljuk az adatokat a bufferbe! Megadjuk az aktu�lisan csatolt buffert, azt hogy h�ny byte adatot m�solunk,
	a m�soland� adatot, majd a feldolgoz�s m�dj�t is meghat�rozzuk: most az adat nem v�ltozik a felt�lt�s ut�n. */
	/* Copy the data to the buffer! First parameter is the currently attached buffer, second is the size of the buffer to be copied,
	third is the array of data, fourth is working mode: now the data can not be modified after this step. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_colors), s_colors, GL_STATIC_DRAW);
	/* Ezen adatok szolg�lj�k a location = 1 vertex attrib�tumot (itt: sz�n).
	   Els�k�nt megadjuk ezt az azonos�t�sz�mot (vertexShader.glsl).
	   Ut�na az attrib�tum m�ret�t (vec3, l�ttuk a shaderben).
	   Harmadik az adat t�pusa.
	   Negyedik az adat normaliz�l�sa, ez maradhat FALSE jelen p�ld�ban.
	   Az attrib�tum �rt�kek hogyan k�vetkeznek egym�s ut�n? Milyen l�p�sk�z ut�n tal�lom a k�vetkez� sz�n adatait?
	   V�g�l megadom azt, hogy honnan kezd�dnek az �rt�kek a pufferben. Most r�gt�n, a legelej�t�l veszem �ket. */
	/* These values are for location = 1 vertex attribute (color).
	   First is the location (vertexShader.glsl).
	   Second is attribute size (vec3, as in the shader).
	   Third is the data type.
	   Fourth defines whether data shall be normalized or not, this is FALSE for the examples of the course.
	   Fifth is the distance in bytes to the next vertex element of the array.
	   Last is the offset of the first color data of the buffer. Now it is the start of the array. */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	/* Enged�lyezz�k az im�nt defini�lt location = 1 attrib�tumot (vertexShader.glsl). */
	/* Enable the previously defined location = 1 attributum (vertexShader.glsl). */
	glEnableVertexAttribArray(1);

	/* Lev�lasztjuk a vertex array objektumot �s a buffert is. */
	/* Detach the vertex array object and the buffer also. */
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/** Aktiv�ljuk a shader-program objektumunkat az alap�rtelmezett fix cs�vezet�k helyett. */
	/** Activate our shader-program object instead of the default fix pipeline. */
	glUseProgram(renderingProgram);

	/** �ll�tsuk be a t�rl�si sz�nt az �ttetsz�s�gi �rt�kkel egy�tt! [0.0, 1.0] */
	/** Set the clear color (red, green, blue, alpha), where alpha is transparency! [0.0, 1.0] */
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(GLFWwindow* window, double currentTime) {
	/* T�r�lj�k le a kiv�lasztott buffereket! */
	/* Let's clear the selected buffers! */
	glClear(GL_COLOR_BUFFER_BIT);

	/* Csatoljuk fel a vertex array objektumot a shader programhoz. */
	/* Bind vertex array object to the shader program. */
	glBindVertexArray(VAO[0]);

	/** Ha pontokat rajzolunk, k�s�rletezz�nk a pont m�ret�nek n�vel�s�vel! */
	/** When drawing points, we can make experiments with different point sizes! */
	glPointSize(30.0);
	/** A megadott adatok seg�ts�g�vel pontokat rajzolunk. */
	/** We draw points with the defined arrays. */
	glDrawArrays(GL_POINTS, 1, 6);
	/** A megadott adatok seg�ts�g�vel h�romsz�get rajzolunk. */
	/** We draw triangle with the defined arrays. */
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	/* Lev�lasztjuk a VAO-t, nehogy b�rmilyen �rt�k fel�l�r�djon. */
	/* Detach VAO for safety reasons, not to modify it accidentaly. */
	glBindVertexArray(0);
}

/** Felesleges objektumok t�rl�se. */
/** Clenup the unnecessary objects. */
void cleanUpScene() {
	/** T�r�lj�k a shader programot. */
	/** Let's delete the shader program. */
	glDeleteProgram(renderingProgram);
	/** T�r�lj�k a GLFW ablakot. */
	/** Destroy the GLFW window. */
	glfwDestroyWindow(window);
	/** Le�ll�tjuk a GLFW-t. */
	/** Stop the GLFW system. */
	glfwTerminate();
	/** Kil�p�s EXIT_SUCCESS k�ddal. */
	/** Stop the software and exit with EXIT_SUCCESS code. */
	exit(EXIT_SUCCESS);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	window_width	= width;
	window_height	= height;

	/** A kezelt k�perny� be�ll�t�sa a teljes (0, 0, width, height) ter�letre. */
	/** Set the viewport for the full (0, 0, width, height) area. */
	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/** ESC billenty�re kil�p�s. */
	/** Exit on ESC key. */
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		cleanUpScene();

	/** A billenty�k lenyom�s�nak �s felenged�s�nek regisztr�l�sa. Lehet�v� teszi gombkombin�ci�k haszn�lat�t. */
	/** Let's register press and release events for keys. Enables the usage of key combinations. */
	if (action == GLFW_PRESS)
		keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE)
		keyboard[key] = GL_FALSE;
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	/** Az eg�r mutat� hely�t kezel� f�ggv�ny. */
	/** Callback function for mouse position change. */
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	/** Az eg�r gombjaihoz k�thet� esem�nyek kezel�se. */
	/** Callback function for mouse button events. */
}

int main(void) {
	/** Pr�b�ljuk meg inicializ�lni a GLFW-t! */
	/** Try to initialize GLFW! */
	if (!glfwInit())
		exit(EXIT_FAILURE);

	/** A haszn�lni k�v�nt OpenGL verzi�: 4.3. */
	/** The needed OpenGL version: 4.3. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed.
	/** Pr�b�ljuk meg l�trehozni az ablakunkat. */
	/** Let's try to create a window for drawing. */
	/** GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor * monitor, GLFWwindow * share) */
	window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);

	/** V�lasszuk ki az ablakunk OpenGL kontextus�t, hogy haszn�lhassuk. */
	/** Select the OpenGL context (window) for drawing. */
	glfwMakeContextCurrent(window);

	/** A k�perny� �tm�retez�s kezel�se. */
	/** Callback function for window size change. */
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	/** Billenty�zethez k�thet� esem�nyek kezel�se. */
	/** Callback function for keyboard events. */
	glfwSetKeyCallback(window, keyCallback);
	/** Az eg�r mutat� hely�t kezel� f�ggv�ny megad�sa. */
	/** Callback function for mouse position change. */
	glfwSetCursorPosCallback(window, cursorPosCallback);
	/** Az eg�r gombjaihoz k�thet� esem�nyek kezel�se. */
	/** Callback function for mouse button events. */
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/** Incializ�ljuk a GLEW-t, hogy el�rhet�v� v�ljanak az OpenGL f�ggv�nyek, probl�ma eset�n kil�p�s EXIT_FAILURE �rt�kkel. */
	/** Initalize GLEW, so the OpenGL functions will be available, on problem exit with EXIT_FAILURE code. */
	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	/** 0 = v-sync kikapcsolva, 1 = v-sync bekapcsolva, n = n db k�pkock�nyi id�t v�rakozunk */
	/** 0 = v-sync off, 1 = v-sync on, n = n pieces frame time waiting */
	glfwSwapInterval(1);

	/** A window ablak minimum �s maximum sz�less�g�nek �s magass�g�nak be�ll�t�sa. */
	/** The minimum and maximum width and height values of the window object. */
	glfwSetWindowSizeLimits(window, 400, 400, 800, 800);
	/** A window oldalar�ny�nak megad�sa a sz�ml�l� �s az oszt� seg�ts�g�vel. */
	/** Setting the aspect ratio using the numerator and the denominator values. */
	glfwSetWindowAspectRatio(window, 1, 1);

	/** Az alkalmaz�shoz kapcsol�d� el�k�sz�t� l�p�sek, pl. a shader objektumok l�trehoz�sa. */
	/** The first initialization steps of the program, e.g.: creating the shader objects. */
	init(window);

	/** A megadott window strukt�ra "close flag" vizsg�lata. */
	/** Checks the "close flag" of the specified window. */
	while (!glfwWindowShouldClose(window)) {
		/** A k�d, amellyel rajzolni tudunk a GLFWwindow objektumunkba. */
		/** Call display function which will draw into the GLFWwindow object. */
		display(window, glfwGetTime());
		/** Double buffered m�k�d�s. */
		/** Double buffered working = swap the front and back buffer here. */
		glfwSwapBuffers(window);
		/** Esem�nyek kezel�se az ablakunkkal kapcsolatban, pl. gombnyom�s. */
		/** Handle events related to our window, e.g.: pressing a key or moving the mouse. */
		glfwPollEvents();
	}

	/** Felesleges objektumok t�rl�se. */
	/** Clenup the unnecessary objects. */
	cleanUpScene();

	/** Kil�p�s EXIT_SUCCESS k�ddal. */
	/** Stop the software and exit with EXIT_SUCCESS code. */
	return EXIT_SUCCESS;
}