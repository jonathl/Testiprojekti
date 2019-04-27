#pragma warning(disable:4996)
#define GLEW_STATIC

#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sstream>
#include <math.h>
#include <time.h>
#include <ctime>
#include <malloc.h>
#include <list>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Noise.h"
#include "3D_object.h"

#include <png.h>
#include <zlib.h>

using namespace glm;

void window_focus_callback(GLFWwindow* window, int focused);
void window_close_callback(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void m_newWindow();
void m_combinePictures(float *pic1, float *pic2, float *texture, float str, int width, int height);
void m_saveAsPNG(char* file_name, int width, int height, float *buffer, char* title);


class My_window {
public:
	char* name;

	#pragma region tyhmiiMuuttujii


	glm::vec3 lightDirection;
	glm::vec3 lightColor;
	float lightAmount = 1.0f;
	glm::vec3 ambientColor;
	float ambientAmount = 1.0f;
	glm::mat3 invTranspose;

	glm::vec3 worldSpaceCameraPos;
	glm::vec3 worldSpaceCameraTarget;
	float shininess = 10.0f;
	glm::vec3 specularColor;
	float specularAmount = 20.0f;

	GLuint programID;
	GLuint MMMatrixID;
	GLuint VMMatrixID;
	GLuint PVMatrixID;
	GLuint AmbientAmountID;
	GLuint AmbientColorID;
	GLuint LightAmountID;
	GLuint LightColorID;
	GLuint LightDirectionID;
	GLuint ITMatrixID;

	GLuint SpecularAmountID;
	GLuint SpecularColorID;
	GLuint WorldSpaceCameraDirID;
	GLuint ShininessID;

	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;

	glm::vec3 cameraDirection = glm::normalize(worldSpaceCameraPos - worldSpaceCameraTarget);

	GLFWwindow* window;
	GLuint VAO;
#pragma endregion
	
	float* texture = NULL;
	int textureWidth;
	int textureHeight;
	GLuint tex;

	My_window(char* n) {
		name = n;
		window = InitWindow();
		glClearColor(0.1f, 0.5f, 0.7f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetWindowFocusCallback(window, window_focus_callback);
		glfwSetWindowCloseCallback(window, window_close_callback);
		setVAO();
		setMVP();
		setProgram();
		setLighting();
	}

	GLFWwindow* InitWindow()
	{
		if (!glfwInit())
		{
			fprintf(stderr, "GLFW failed!\n");
			return NULL;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		GLFWwindow* window = NULL;
		window = glfwCreateWindow(724, 724, "Testiprojekti", NULL, NULL);
		if (window == NULL) {
			fprintf(stderr, "Failed to open GLFW window.\n");
			glfwTerminate();
			return NULL;
		}

		glfwMakeContextCurrent(window);

		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			getchar();
			glfwTerminate();
			return NULL;
		}

		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		return window;
	}
	void setProgram() {
		programID = LoadShaders("VertexShader.vertexshader", "FragmentShader.fragmentshader");
		MMMatrixID = glGetUniformLocation(programID, "MM");
		VMMatrixID = glGetUniformLocation(programID, "VM");
		PVMatrixID = glGetUniformLocation(programID, "PV");
		AmbientAmountID = glGetUniformLocation(programID, "AmbientAmount");
		AmbientColorID = glGetUniformLocation(programID, "AmbientColor");
		LightAmountID = glGetUniformLocation(programID, "LightAmount");
		LightColorID = glGetUniformLocation(programID, "LightColor");
		LightDirectionID = glGetUniformLocation(programID, "LightDirection");
		ITMatrixID = glGetUniformLocation(programID, "IT");

		SpecularAmountID = glGetUniformLocation(programID, "SpecularAmount");
		SpecularColorID = glGetUniformLocation(programID, "SpecularColor");
		WorldSpaceCameraDirID = glGetUniformLocation(programID, "WorldSpaceCameraDir");
		ShininessID = glGetUniformLocation(programID, "Shininess");

	}
	void setVAO() {
		glfwMakeContextCurrent(window);
		float vertices[] = {
			//  Position      Color             Texcoords
			-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Top-left
			1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Top-right
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
			1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f  // Bottom-right
		};

		float normals[] = {
			0.0f, 0.0f, -1.0f, // Top-left
			0.0f, 0.0f, -1.0f, // Top-right
			0.0f, 0.0f, -1.0f, // Bottom-right
			0.0f, 0.0f, -1.0f // Bottom-left
		};

		//-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Top-left
		//	1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Top-right
		//	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-right
		//	1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f  // Bottom-left

		unsigned int indices[] = {
			0, 1, 2, // first triangle
			1, 2, 3  // second triangle
		};

		unsigned int VNO, VBO, mVAO, EBO;
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &VNO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(mVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VNO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
		// normal attribute
		glBindBuffer(GL_ARRAY_BUFFER, VNO);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(3);

		VAO = mVAO;
	}
	void setMVP() {
		Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);

		worldSpaceCameraPos.x = 0.0f;
		worldSpaceCameraPos.y = 0.0f;
		worldSpaceCameraPos.z = -2.4250f; //2
		worldSpaceCameraTarget.x = 0.0f;
		worldSpaceCameraTarget.y = 0.0f;
		worldSpaceCameraTarget.z = 1.0f;

		View = glm::lookAt(
			worldSpaceCameraPos,
			worldSpaceCameraTarget,
			glm::vec3(1, 0, 0)
		);

		Model = glm::mat4(3.0f);
	}
	void setLighting() {
		ambientColor.r = 1.0f;
		ambientColor.g = 1.0f;
		ambientColor.b = 1.0f;

		lightDirection.x = 0.0f;
		lightDirection.y = 0.0f;
		lightDirection.z = 1.0f;

		lightColor.x = 1.0f;
		lightColor.y = 1.0f;
		lightColor.z = 1.0f;

		specularColor.x = 1.0f;
		specularColor.y = 1.0f;
		specularColor.z = 1.0f;
	}
	void setTexture(int wi, int he, float* pic) {
		texture = pic;
		textureWidth = wi;
		textureHeight = he;
		glfwMakeContextCurrent(window);
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wi, he, 0, GL_RGB, GL_FLOAT, pic);
	}
	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

		// Create the shaders
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
		if (VertexShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << VertexShaderStream.rdbuf();
			VertexShaderCode = sstr.str();
			VertexShaderStream.close();
		}
		else {
			printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
			getchar();
			return 0;
		}

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
		if (FragmentShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << FragmentShaderStream.rdbuf();
			FragmentShaderCode = sstr.str();
			FragmentShaderStream.close();
		}

		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Vertex Shader
		printf("Compiling shader : %s\n", vertex_file_path);
		char const * VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			printf("%s\n", &VertexShaderErrorMessage[0]);
		}

		// Compile Fragment Shader
		printf("Compiling shader : %s\n", fragment_file_path);
		char const * FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			printf("%s\n", &FragmentShaderErrorMessage[0]);
		}

		// Link the program
		printf("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
		}

		glDetachShader(ProgramID, VertexShaderID);
		glDetachShader(ProgramID, FragmentShaderID);

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		return ProgramID;
	}

	void mainLoop() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);


		glUniformMatrix4fv(MMMatrixID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(VMMatrixID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(PVMatrixID, 1, GL_FALSE, &Projection[0][0]);

		invTranspose = glm::transpose(glm::inverse(glm::mat3(Model)));
		glUniformMatrix3fv(ITMatrixID, 1, GL_FALSE, &invTranspose[0][0]);



		glBindTexture(GL_TEXTURE_2D, tex);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



		glUniform1f(AmbientAmountID, ambientAmount);
		glUniform3f(AmbientColorID, ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform1f(LightAmountID, lightAmount);
		glUniform3f(LightColorID, lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(LightDirectionID, lightDirection.x, lightDirection.y, lightDirection.z);
		glUniform3f(SpecularColorID,
			specularColor.x,
			specularColor.y,
			specularColor.z);
		glUniform3f(WorldSpaceCameraDirID,
			cameraDirection.x,
			cameraDirection.y,
			cameraDirection.z);
		glUniform1f(
			ShininessID,
			shininess
		);
		glUniform1f(
			SpecularAmountID,
			specularAmount
		);
		glfwSwapBuffers(window);
	}

	void closeWindow() {

	}
};

double cursor_xpos, cursor_ypos;
bool cursorLeftPressed = false;
bool cursorRightPressed = false;
My_window* currentWindow;
My_window* previousWindow;
std::vector<My_window*> windows;
bool closeProgram = false;
bool ctrlDown = false;
bool lineMode = false;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		cursorRightPressed = true;
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		cursorRightPressed = false;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		cursorLeftPressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		cursorLeftPressed = false;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		for (int i = 0; i < windows.size(); ++i) {
			if (windows[i]->window == window) {
				std::cout << windows[i]->name;
				break;
			}
		}
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		if (previousWindow != NULL) {
			My_window* p = previousWindow;
			My_window* c = currentWindow;
			m_newWindow();
			windows.back()->texture = new float[c->textureWidth * c->textureHeight * 3];
			m_combinePictures(c->texture, p->texture, windows.back()->texture, 0.5f, c->textureWidth, c->textureHeight);
			windows.back()->setTexture(c->textureWidth, c->textureHeight, windows.back()->texture);
		}
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		for (int i = 0; i < windows.size(); ++i) {
			if (windows[i]->window == window) {
				int wi = windows[i]->textureWidth;
				int he = windows[i]->textureHeight;
				if (windows[i]->texture == NULL) {
					wi = 700;
					he = 700;
					windows[i]->texture = new float[wi*he * 3];
				}
				PerlinNoise pn(wi, he, 6);
				pn.m_genPerlinNoise(windows[i]->texture, 3);
				windows[i]->setTexture(wi, he, windows[i]->texture);
				break;
			}
		}
	}

	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		for (int i = 0; i < windows.size(); ++i) {
			if (windows[i]->window == window) {
				int wi = windows[i]->textureWidth;
				int he = windows[i]->textureHeight;
				if (windows[i]->texture == NULL) {
					wi = 700;
					he = 700;
					windows[i]->texture = new float[wi*he * 3];
				}
				WorleyNoise pn(wi, he, 8, 6);
				pn.m_genWorleyNoise(windows[i]->texture, 3);
				windows[i]->setTexture(wi, he, windows[i]->texture);
				break;
			}
		}
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		closeProgram = true;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS && ctrlDown) {
		m_newWindow();
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS && ctrlDown) {
		window_close_callback(currentWindow->window);
	}
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		ctrlDown = true;
	}
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
		ctrlDown = false;
	}
}

void window_focus_callback(GLFWwindow* window, int focused) {
	if (focused) {
		for (int i = 0; i < windows.size(); ++i) {
			if (windows[i]->window == window) {
				previousWindow = currentWindow;
				currentWindow = windows[i];
				glfwMakeContextCurrent(currentWindow->window);
			}
		}
	}
}

void window_close_callback(GLFWwindow* window) {
	for (int i = 0; i < windows.size(); ++i) {
		if (windows[i]->window == window) {
			delete windows[i];
			windows.erase(windows.begin() +i);
		}
	}
	if (windows.size() != 0) {
		glfwFocusWindow(windows[0]->window);
	}
	else {
		currentWindow = NULL;
	}
	glfwDestroyWindow(window);
}

int m_getTextureCoordinate(int width, int height, int x, int y) {
	int textcoord = width * (height-y) - x;
	//std::cout << "texcoord = " << textcoord << "\n";
	return textcoord;
}

void m_updateTexture(GLuint tex, float *texture, int width, int height) {
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, texture);
}

void m_drawToTexture(GLuint tex,float *texture, int width, int height) { //korjaa reunan yli piirtäminen ja textuurin ko-on vaikuttaminen
	float color[] = { 1.0f, 0.0f, 0.0f };
	int index = m_getTextureCoordinate(width, height, (int)cursor_xpos, (int)cursor_ypos) * 3;
	texture[index] = color[0];
	texture[++index] = color[1];
	texture[++index] = color[2];
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, texture);
}

void m_colorAdjacent(float *color, float *fillcolor, int texel, float *texture, int width, int height) { //poista recursio, stackki täyttyy
	if ((color[0] != fillcolor[0]) && (color[1] != fillcolor[1]) && (color[2] != fillcolor[2])) {
		texture[texel] = fillcolor[0];
		texture[texel+1] = fillcolor[1];
		texture[texel+2] = fillcolor[2];
		if (texel > width * 3) { //up
			m_colorAdjacent(color, fillcolor, texel - width*3, texture, width, height);
		}
		if (texel % (width * 3) != 0) { //left
			m_colorAdjacent(color, fillcolor, texel - 3, texture, width, height);
		}
		if (texel % (width * 3) != (width * 3) - 1) { //right
			m_colorAdjacent(color, fillcolor, texel + 3, texture, width, height);
		}
		if (texel < width * 3 * (height-1)) { //down
			m_colorAdjacent(color, fillcolor, texel + width*3, texture, width, height);
		}
	}
}

void m_bucketToolTexture(GLuint tex, float *texture, int width, int height) {
	float fillcolor[] = { 1.0f, 1.0f, 1.0f };
	int index = m_getTextureCoordinate(width, height, (int)cursor_xpos, (int)cursor_ypos) * 3;
	float color[] = { texture[index], texture[index+1], texture[index+2] };// onnistuu ylittämään max arvon??
	m_colorAdjacent(color, fillcolor, index, texture, width, height);
	m_updateTexture(tex, texture, width, height);
}

void m_bucketToolTexture2(GLuint tex, float *texture, int width, int height) {
	float fillcolor[] = { 1.0f, 1.0f, 1.0f };
	int o = m_getTextureCoordinate(width, height, (int)cursor_xpos, (int)cursor_ypos) * 3;
	int index = o;
	int oy = o;
	float color[] = { texture[index], texture[index + 1], texture[index + 2] };

	while ((texture[index] != fillcolor[0]) && (texture[index + 1] != fillcolor[1]) && (texture[index + 2] != fillcolor[2])) { //vertaa coloriin
		while ((texture[index] != fillcolor[0]) && (texture[index + 1] != fillcolor[1]) && (texture[index + 2] != fillcolor[2])) {
			texture[index] = fillcolor[0];
			texture[index + 1] = fillcolor[1];
			texture[index + 2] = fillcolor[2];
			index += 3;
		}
		oy += width * 3;
		index = oy;
	}
	
	m_updateTexture(tex, texture, width, height);
}

//GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {
//
//	// Create the shaders
//	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
//	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
//
//	// Read the Vertex Shader code from the file
//	std::string VertexShaderCode;
//	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
//	if (VertexShaderStream.is_open()) {
//		std::stringstream sstr;
//		sstr << VertexShaderStream.rdbuf();
//		VertexShaderCode = sstr.str();
//		VertexShaderStream.close();
//	}
//	else {
//		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
//		getchar();
//		return 0;
//	}
//
//	// Read the Fragment Shader code from the file
//	std::string FragmentShaderCode;
//	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
//	if (FragmentShaderStream.is_open()) {
//		std::stringstream sstr;
//		sstr << FragmentShaderStream.rdbuf();
//		FragmentShaderCode = sstr.str();
//		FragmentShaderStream.close();
//	}
//
//	GLint Result = GL_FALSE;
//	int InfoLogLength;
//
//	// Compile Vertex Shader
//	printf("Compiling shader : %s\n", vertex_file_path);
//	char const * VertexSourcePointer = VertexShaderCode.c_str();
//	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
//	glCompileShader(VertexShaderID);
//
//	// Check Vertex Shader
//	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
//		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
//		printf("%s\n", &VertexShaderErrorMessage[0]);
//	}
//
//	// Compile Fragment Shader
//	printf("Compiling shader : %s\n", fragment_file_path);
//	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
//	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
//	glCompileShader(FragmentShaderID);
//
//	// Check Fragment Shader
//	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
//		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
//		printf("%s\n", &FragmentShaderErrorMessage[0]);
//	}
//
//	// Link the program
//	printf("Linking program\n");
//	GLuint ProgramID = glCreateProgram();
//	glAttachShader(ProgramID, VertexShaderID);
//	glAttachShader(ProgramID, FragmentShaderID);
//	glLinkProgram(ProgramID);
//
//	// Check the program
//	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
//	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
//		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
//		printf("%s\n", &ProgramErrorMessage[0]);
//	}
//
//	glDetachShader(ProgramID, VertexShaderID);
//	glDetachShader(ProgramID, FragmentShaderID);
//
//	glDeleteShader(VertexShaderID);
//	glDeleteShader(FragmentShaderID);
//
//	return ProgramID;
//}

//GLFWwindow* InitWindow()
//{
//	if (!glfwInit())
//	{
//		fprintf(stderr, "GLFW failed!\n");
//		return NULL;
//	}
//
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
//
//	GLFWwindow* window = NULL;
//	window = glfwCreateWindow(724, 724, "Testiprojekti", NULL, NULL);
//	if (window == NULL) {
//		fprintf(stderr, "Failed to open GLFW window.\n");
//		glfwTerminate();
//		return NULL;
//	}
//
//	glfwMakeContextCurrent(window);
//
//	if (glewInit() != GLEW_OK) {
//		fprintf(stderr, "Failed to initialize GLEW\n");
//		getchar();
//		glfwTerminate();
//		return NULL;
//	}
//
//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
//	return window;
//}

void m_genOneColorTex(int width, int height, float *texture, float r, float g, float b) {
	int index = -1;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			texture[++index] = r;
			texture[++index] = g;
			texture[++index] = b;
		}
	}
}

void m_genCheckerboardTex(int width, int height, float *texture) {
	int state = 1;
	int others = 1;
	int index = 0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (state == others) {
				texture[index] = 1.0f;
				texture[index+1] = 1.0f;
				texture[index+2] = 1.0f;
				index += 3;
			}
			else {
				texture[index] = 0.0f;
				texture[index + 1] = 0.0f;
				texture[index + 2] = 0.0f;
				index += 3;
			}
			state *= -1;
		}
		others *= -1;
	}
}

void m_drawGridOnTex(int width, int height, int gridsize, float *texture) {
	int index = 0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (j % gridsize == 0 || i % gridsize == 0) {
				texture[index] = 1.0f;
				texture[index + 1] = 0.2f;
				texture[index + 2] = 0.7f;
			}
			index += 3;
		}
	}
}

void m_drawLine(int x0, int y0, int x1, int y1, float *texture, int width, int height) {

	int index2 = m_getTextureCoordinate(width, height, x1, y1) * 3;
	texture[index2] = 1.0f;
	texture[index2+ 1] = 0.2f;
	texture[index2 + 2] = 0.4f;
	index2 = m_getTextureCoordinate(width, height, x0, y0) * 3;
	texture[index2] = 1.0f;
	texture[index2 + 1] = 0.2f;
	texture[index2 + 2] = 0.4f;


	int y2 = y1 - y0;
	int x2 = x1 - x0;
	float a = (float)y2 / (float)x2; //y = ax + b
	float b = y0 - a * x0;
	int m = 1;
	int i = 0;
	float x = x0, y = y0;
	if (x2 == 0) { //x0 = x1
		if (y2 < 0) {
			m = -1;
		}
		while ((round(x) != x1 || round(y) != y1))
		{
			if (i != 0) {
				y += 1 * m;
			}
			int index = m_getTextureCoordinate(width, height, x, y) * 3;
			texture[index] = 1.0f;
			texture[index + 1] = 0.2f;
			texture[index + 2] = 0.4f;
			++i;
		}
	} 

	else if (y2 == 0) { //y0 = y1
		if (x2 < 0) {
			m = -1;
		}
		while ((round(x) != x1 || round(y) != y1))
		{
			if (i != 0) {
				x += 1*m;
			}
			int index = m_getTextureCoordinate(width, height, x, y) * 3;
			texture[index] = 1.0f;
			texture[index + 1] = 0.2f;
			texture[index + 2] = 0.4f;
			++i;
		}
	}

	else if (a <= 1 && a >= -1) {
		if (x2 < 0) {
			m = -1;
		}
		while((round(x) != x1 || round(y) != y1))
		{
			x += 1 * m;
			y = roundf(a * x + b);
			int index = m_getTextureCoordinate(width, height, round(x), y) *3;
			texture[index] = 1.0f;
			texture[index + 1] = 0.2f;
			texture[index + 2] = 0.4f;
			++i;
		}
	}

	else {
		if (y2 < 0) {
			m = -1;
		}
		while ((round(x) != x1 || round(y) != y1))
		{
			y += 1 * m;
			x = roundf((y - b)/a);
			int index = m_getTextureCoordinate(width, height, round(x), y) * 3;
			texture[index] = 1.0f;
			texture[index + 1] = 0.2f;
			texture[index + 2] = 0.4f;
			++i;
		}
	}
}

void m_drawLine(int2 c0, int2 c1, float *texture, int width, int height) {
	m_drawLine(c0.x, c0.y, c1.x, c1.y, texture, width, height);
}

void m_drawFractal(FractalLine f, float *texture, int width, int height) {
	if (f.fl.empty()) {
		m_drawLine(f.c0, f.c1, texture, width, height);
	}
	else {
		for each(FractalLine* fra in f.fl) {
			m_drawFractal(*fra, texture, width, height);
		}
	}
}

void m_combinePictures(float *pic1, float *pic2, float *texture, float str, int width, int height) {
	for (int i = 0; i < height*width * 3; ++i)
	{
		texture[i] = pic1[i] + str * (pic2[i] - pic1[i]);
	}
	/*int index = -1;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			texture[++index] = pic1[++index] + str * (pic2[++index] - pic1[++index]);
			texture[++index] = pic1[++index] + str * (pic2[++index] - pic1[++index]);
			texture[++index] = pic1[++index] + str * (pic2[++index] - pic1[++index]);
		}
	}*/
}

float m_changeScale(float oldmin, float oldmax, float newmin, float newmax, float value) {
	float p = (value - oldmin) / (oldmax - oldmin);
	return p * (newmax - newmin) + newmin;
}

void m_addContrast(float *texture, float str, float displacement, int width, int height) {
	if (str == 0) {
		return;
	}
	float min = atanf(2 * 3.141 * str * (-0.5f + displacement)), max = atanf(2 * 3.141 * str *  (0.5f + displacement));
	float value;
	for (int i = 0; i < height*width * 3; ++i)
	{
		value = atanf(2 * 3.141 * str * (texture[i] - 0.5f + displacement));
		texture[i] = m_changeScale(min, max, 0, 1, value);
	}
}

//void m_genRandomNoise(int width, int height, float *texture) {
//	int index = -1;
//	float r_value;
//	for (int i = 0; i < height; ++i) {
//		for (int j = 0; j < width; ++j) {
//			r_value = float(rand() % 100)/100;
//			texture[++index] = r_value;
//			texture[++index] = r_value;
//			texture[++index] = r_value;
//		}
//	}
//}


void write_row_callback(png_structp png_ptr, png_uint_32 row, int pass){
	//std::cout << row <<"\n";
}
void m_saveAsPNG(char* file_name, int width, int height, float *buffer, char* title) {
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	FILE *fp = fopen(file_name, "wb");
	if (!fp) {
		std::printf("[write_png_file] File %s could not be opened for writing", file_name);
		return;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		printf("Could not allocate write struct\n");
		return;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		printf("Could not allocate info struct\n");
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error during png creation\n");
		fclose(fp);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return;
	}

	png_init_io(png_ptr, fp);

	png_set_write_status_fn(png_ptr, write_row_callback);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	if (title != NULL) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "Title";
		title_text.text = title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}
	png_write_info(png_ptr, info_ptr);


	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep)malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y = 0; y<height; ++y) {
		for (x = 0; x<width; ++x) {
			int i = x * 3;
			row[i] = png_byte(int(buffer[y * width * 3 + i] * 255));
			row[i+1] = png_byte(int(buffer[y * width * 3 + i+1] * 255));
			row[i+2] = png_byte(int(buffer[y * width * 3 + i+2] * 255));
		}
		png_write_row(png_ptr, row);
		//rows[y] = row;
	}
	//png_write_image(png_ptr, rows);
	// End write
	png_write_end(png_ptr, NULL);

	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);
}

void m_readPNG_image() {
	png_image image;
	memset(&image, 0, (sizeof image));
	image.version = PNG_IMAGE_VERSION;
	if (png_image_begin_read_from_file(&image, "helmet_kawaii.png") != 0) {
		png_bytep buffer;
		image.format = PNG_FORMAT_RGBA;
		buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(image));
		if (buffer != NULL && png_image_finish_read(&image, NULL, buffer, 0, NULL) != 0) {
			if (png_image_write_to_file(&image, "kuva1.png", 0, buffer, 0, NULL) != 0) {
				std::cout << "we did it \n";
			}
		}
		else {
			if (buffer == NULL) {
				png_image_free(&image);
			}
			else {
				free(buffer);
			}
		}
	}
}

void m_readPNG(char* file_name, png_structp png_ptr, png_infop info_ptr, png_bytepp row_pointers) {
	FILE *fp = fopen(file_name, "rb");
	if (!fp) {
		return;
	}
	unsigned char header[9];
	int number_to_check = 8;
	fread(header, 1, number_to_check, fp);
	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png) {
		return;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		return;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
	}
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	}
	png_init_io(png_ptr, fp);

	png_set_sig_bytes(png_ptr, number_to_check);

	png_read_info(png_ptr, info_ptr);
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type, compression_type, filter_method;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

	row_pointers = (png_bytepp)png_malloc(png_ptr, sizeof(png_bytepp) * height);
	for (int i = 0; i < height; i++) {
		row_pointers[i] = (png_bytep)png_malloc(png_ptr, width * sizeof(png_bytep));
	}

	png_set_rows(png_ptr, info_ptr, row_pointers);
	
	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, end_info);

	std::cout << (int)row_pointers[1][1] << "\n";

	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
}

void m_genCube() {

}

void m_genPlane(int x, int y) {
	int size = x * y * 3;
	float* verts = new float[size];
	int index = -1;
	for (int i = 0; i < y; ++i) {
		for (int j = 0; j < x; ++j) {
			verts[++index] = j;
			verts[++index] = i;
			verts[++index] = 0;
		}
	}

	for (int i = 0; i < size; ++i) {
		verts[i] /= x;
	}

	int faces = (x - 1)*(y - 1);
	float* indices = new float[faces*2*3];
	index = -1;
	for (int i = 0; i < y - 1; ++i) {
		for (int j = 0; j < x - 1; ++i) {
			indices[++index] = j + i*j;
			indices[++index] = j + 1 + (i + 1)*j;
			indices[++index] = j + (i + 1)*j;
			indices[++index] = j + i*j;
			indices[++index] = j + 1 + i*j;
			indices[++index] = j + 1 + (i + 1)*j;
		}
	}
}

GLuint setVAO() {

	float vertices[] = {
		//  Position      Color             Texcoords
		-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Top-left
		1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Top-right
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
		1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f  // Bottom-right
	};

	float normals[] = {
		0.0f, 0.0f, -1.0f, // Top-left
		0.0f, 0.0f, -1.0f, // Top-right
		0.0f, 0.0f, -1.0f, // Bottom-right
		0.0f, 0.0f, -1.0f // Bottom-left
	};

	//-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Top-left
	//	1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Top-right
	//	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-right
	//	1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f  // Bottom-left

	unsigned int indices[] = {
		0, 1, 2, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VNO, VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VNO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VNO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, VNO);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);

	return VAO;
}

void m_newWindow() {
	char* n = "window";
	n += 'w';
	windows.push_back(new My_window(n));
	previousWindow = currentWindow;
	currentWindow = windows.back();
	glfwMakeContextCurrent(windows.back()->window);
}

int main()
{
	srand(time(NULL));
	My_window* window = new My_window("window1");
	My_window* w2 = new My_window("window2");
	windows.push_back(window);
	windows.push_back(w2);
	glfwMakeContextCurrent(window->window);
	currentWindow = window;

	const int wi = 700, he = 700;

	float* pic = new float[wi*he * 3];
	float* pic2 = new float[wi*he * 3];

	PerlinNoise pn(wi, he, 6);
	WorleyNoise wn(wi, he, 8, 5);

	wn.m_genWorleyNoise(pic2, 4);
	clock_t begin = clock();
	pn.m_genPerlinNoise(pic,3);
	clock_t end = clock();
	std::cout << double(end - begin) / CLOCKS_PER_SEC << "\n";
	m_saveAsPNG("perlin.png", wi, he, pic, "k");
	m_addContrast(pic2, 0.2f, 0.3f, wi, he);
	m_saveAsPNG("worley.png", wi, he, pic2, "k");
	m_combinePictures(pic, pic2, pic, 0.4f, wi, he);

	m_genOneColorTex(wi, he, pic, 0.7f, 0.7f, 0.9f);
	//m_drawGridOnTex(wi, he, 125, pic);
	FractalLine f(200, 200, 500, 200);
	f.addLine(500, 500);
	f.addLine(200, 500);
	//int2 coor[] = { int2(200,500) };
	//f.addLines(coor, 1);
	//f.closeLines();
	//for (int r = 0; r < 3; ++r) {
	//	//std::cout << "\nITERATE START!\n\n";
	//	f.iterFractal();
	//}
	////f.printPoints();
	m_drawFractal(f, pic, wi, he);
	//m_drawLine(f.c0.x, f.c0.y, f.c1.x, f.c1.y, pic, wi, he);
	m_saveAsPNG("kuva.png", wi, he, pic, "k");

	window->setTexture(wi, he, pic);
	w2->setTexture(wi, he, pic2);

	do {
		currentWindow->mainLoop(); // windows[i]->mainloop();

		glfwPollEvents();

		if (currentWindow != NULL) {

			if (cursorLeftPressed) {
				/*glfwGetCursorPos(currentWindow->window, &cursor_xpos, &cursor_ypos);
				m_drawToTexture(currentWindow->tex, pic, wi, he);*/
			}
			if (cursorRightPressed) {
				/*glfwGetCursorPos(currentWindow->window, &cursor_xpos, &cursor_ypos);
				m_bucketToolTexture2(currentWindow->tex, pic, wi, he);*/
			}
			
		}
	} while (windows.size() != 0 && !closeProgram);// && glfwWindowShouldClose(currentWindow->window) == 0);

	glfwTerminate();
}