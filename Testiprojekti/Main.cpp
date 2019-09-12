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

#include "3D_object.h"
#include "Texture.h"

#include <png.h>
#include <zlib.h>

using namespace glm;

void window_focus_callback(GLFWwindow* window, int focused);
void window_close_callback(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void m_newWindow();

//void m_genPlane(int x, int y, float* verts, unsigned int* indices, float* texcoord, float* normals);
Object3D* m_genPlane(int x, int y);


class My_window {
public:
	char* name;
	int indicesSize;

	#pragma region tyhmiiMuuttujii

	int windowPixelSizeX = 724;
	int windowPixelSizeY = 724;

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
	std::vector<glm::mat4> Models;

	glm::vec3 cameraDirection = glm::normalize(worldSpaceCameraPos - worldSpaceCameraTarget);

	GLFWwindow* window;
#pragma endregion
	

	std::vector<GLuint> VAO;
	std::vector<GLuint> tex;
	std::vector<Object3D*> obj3D;

	/*float* texture = NULL;
	int textureWidth;
	int textureHeight;*/

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
		//setVAO();
		//setMVP();
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
		window = glfwCreateWindow(windowPixelSizeX, windowPixelSizeY, "Testiprojekti", NULL, NULL);
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
		int size = obj3D.back()->verticesSize;
		float* texcoord = new float[size * 2]; //‰l‰ tee aina uusia
		float* normals = new float[size * 3];
		float* vertices = new float[size * 3];
		indicesSize = obj3D.back()->tris.size() * 3;
		unsigned int* indices = new unsigned int[indicesSize];

		int index = -1;
		int index2 = -1;
		for (int i = 0; i < size; ++i) {
			float3 vert = float3(obj3D.back()->verts[i]->x, obj3D.back()->verts[i]->y, obj3D.back()->verts[i]->z);
			vertices[++index] = vert.x; 
			normals[index] = 1;
			vertices[++index] = vert.y;
			normals[index] = 1;
			vertices[++index] = vert.z; 
			normals[index] = 1;
			texcoord[++index2] = obj3D.back()->verts[i]->u;
			texcoord[++index2] = obj3D.back()->verts[i]->v;
		}
		index = -1;
		for (int i = 0; i < indicesSize / 3; ++i) {
			indices[++index] = obj3D.back()->tris[i]->corners[0].index;
			indices[++index] = obj3D.back()->tris[i]->corners[1].index;
			indices[++index] = obj3D.back()->tris[i]->corners[2].index;
		}

		unsigned int VNO, VBO, mVAO, EBO, VTC;
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &VNO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenBuffers(1, &VTC);

		glBindVertexArray(mVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VNO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size * 3, normals, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size * 3, vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VTC); //texcoord
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size * 2, texcoord, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesSize, indices, GL_STATIC_DRAW);

		// position attribute
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coord attribute
		glBindBuffer(GL_ARRAY_BUFFER, VTC);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		// normal attribute
		glBindBuffer(GL_ARRAY_BUFFER, VNO);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(3);

		delete[] texcoord;
		delete[] normals;
		delete[] vertices;
		delete[] indices;

		VAO.push_back(mVAO);
	}

	void setMVP() {
		Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);

		worldSpaceCameraPos.x = 0.5f;
		worldSpaceCameraPos.y = 0.5f;
		worldSpaceCameraPos.z = -1.2f; //-1.2
		worldSpaceCameraTarget.x = 0.5f;
		worldSpaceCameraTarget.y = 0.5f;
		worldSpaceCameraTarget.z = 0.0f;


		View = glm::lookAt(
			worldSpaceCameraPos,
			worldSpaceCameraTarget,
			glm::vec3(0, 1, 0)
		);


		//Flips view x-axis
		View = glm::scale(View, glm::vec3(-1, 1, 1));
		View = glm::translate(View, glm::vec3(-1, 0, 0));

		SetModelMatrix();
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
	/*void setTexture(Object3D o, int wi, int he, float* pic) {
		o.SetTexture(wi, he, pic);
		GLuint t;
		glfwMakeContextCurrent(window);
		glGenTextures(1, &t);
		glBindTexture(GL_TEXTURE_2D, t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, o.textureWidth, o.textureHeight, 0, GL_RGB, GL_FLOAT, o.texture);
		tex.push_back(t);
	}*/
	void setTexture(Object3D o, Texture texture) {
		//o.SetTexture();
		GLuint t; 
		glfwMakeContextCurrent(window);
		glGenTextures(1, &t);
		glBindTexture(GL_TEXTURE_2D, t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_FLOAT, texture.pic);
		tex.push_back(t);
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

	void SetModelMatrix() {
		Object3D obj = *obj3D.back();
		glm::mat4 Model = glm::mat4(1.0f);
		//translate
		glm::mat4 mTranslate = glm::translate(Model, glm::vec3(obj.positionFactor->x, obj.positionFactor->y, obj.positionFactor->z));
		//rotate
		glm::mat4 mRotate = glm::rotate(Model, radians(obj.rotateFactor->z), glm::vec3(0, 0, 1)) * glm::rotate(Model, radians(obj.rotateFactor->y), glm::vec3(0, 1, 0)) * glm::rotate(Model, radians(obj.rotateFactor->x), glm::vec3(1, 0, 0));
		glm::vec4 rotatePivot = mRotate * vec4(obj.pivotPoint->x, obj.pivotPoint->y, obj.pivotPoint->z, 1);
		glm::mat4 mTranslateRotatePivot = glm::translate(Model, glm::vec3(-(rotatePivot[0] - obj.pivotPoint->x), -(rotatePivot[1] - obj.pivotPoint->y), -(rotatePivot[2] - obj.pivotPoint->z)));
		//scale
		glm::mat4 mScale = glm::scale(Model, glm::vec3(obj.scaleFactor->x, obj.scaleFactor->y, obj.scaleFactor->z));
		glm::mat4 mTranslateScalePivot = glm::translate(Model, glm::vec3(-obj.pivotPoint->x * (obj.scaleFactor->x-1), -obj.pivotPoint->y * (obj.scaleFactor->y-1), -obj.pivotPoint->z * (obj.scaleFactor->z-1)));

		Model = Model * mTranslate * mTranslateRotatePivot * mRotate * mTranslateScalePivot * mScale;
		Models.push_back(Model);
	}

	void SetModelMatrix(int i) {
		Object3D obj = *obj3D[i];
		glm::mat4 Model = glm::mat4(1.0f);
		//translate
		glm::mat4 mTranslate = glm::translate(Model, glm::vec3(obj.positionFactor->x, obj.positionFactor->y, obj.positionFactor->z));
		//rotate
		glm::mat4 mRotate = glm::rotate(Model, radians(obj.rotateFactor->z), glm::vec3(0, 0, 1)) * glm::rotate(Model, radians(obj.rotateFactor->y), glm::vec3(0, 1, 0)) * glm::rotate(Model, radians(obj.rotateFactor->x), glm::vec3(1, 0, 0));
		glm::vec4 rotatePivot = mRotate * vec4(obj.pivotPoint->x, obj.pivotPoint->y, obj.pivotPoint->z, 1);
		glm::mat4 mTranslateRotatePivot = glm::translate(Model, glm::vec3(-(rotatePivot[0] - obj.pivotPoint->x), -(rotatePivot[1] - obj.pivotPoint->y), -(rotatePivot[2] - obj.pivotPoint->z)));
		//scale
		glm::mat4 mScale = glm::scale(Model, glm::vec3(obj.scaleFactor->x, obj.scaleFactor->y, obj.scaleFactor->z));
		glm::mat4 mTranslateScalePivot = glm::translate(Model, glm::vec3(-obj.pivotPoint->x * (obj.scaleFactor->x - 1), -obj.pivotPoint->y * (obj.scaleFactor->y - 1), -obj.pivotPoint->z * (obj.scaleFactor->z - 1)));

		Model = Model * mTranslate * mTranslateRotatePivot * mRotate * mTranslateScalePivot * mScale;
		Models[i] = (Model);
	}

	void mainLoop() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//shaderit
		glUseProgram(programID);

		//ModelViewProjection Matriisit
		for(int i = 0; i < VAO.size(); ++i)
		{
			glUniformMatrix4fv(MMMatrixID, 1, GL_FALSE, &Models[i][0][0]); // &Model[0][0]
			glUniformMatrix4fv(VMMatrixID, 1, GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(PVMatrixID, 1, GL_FALSE, &Projection[0][0]);

			invTranspose = glm::transpose(glm::inverse(glm::mat3(Models[i])));
			glUniformMatrix3fv(ITMatrixID, 1, GL_FALSE, &invTranspose[0][0]);

			glBindTexture(GL_TEXTURE_2D, tex[i]); //texturit voi varmaan asettaa eri j‰rjestykses

			glBindVertexArray(VAO[i]);

			glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
		}



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

	void AddObject3D(Object3D* object) {
		obj3D.push_back(object);
		setVAO();
		setMVP();
	}

	void closeWindow() {

	}
};

Object3D RayHitObject(My_window* w);

double cursor_xpos, cursor_ypos;
bool cursorLeftPressed = false;
bool cursorRightPressed = false;
My_window* currentWindow;
My_window* previousWindow;
std::vector<My_window*> windows;
bool closeProgram = false;
bool ctrlDown = false;
bool lineMode = false;


#pragma region WindowCallbacks
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		cursorRightPressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		cursorRightPressed = false;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		RayHitObject(currentWindow);
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

	if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
		for (int i = 0; i < windows.size(); ++i) {
			if (windows[i]->window == window) {
				windows[i]->obj3D.back()->AddScale(float3(0.05f,0.05f,0));
				windows[i]->SetModelMatrix(1);
				break;
			}
		}
	}

	if (key == GLFW_KEY_9 && action == GLFW_PRESS) {
		for (int i = 0; i < windows.size(); ++i) {
			if (windows[i]->window == window) {
				windows[i]->obj3D.back()->DecScale(float3(0.05f, 0.05f, 0));
				windows[i]->SetModelMatrix(1);
				break;
			}
		}
	}

	if (key == GLFW_KEY_8 && action == GLFW_REPEAT) {
		for (int i = 0; i < windows.size(); ++i) {
			if (windows[i]->window == window) {
				windows[i]->obj3D.back()->AddRotate(0, 0, 5);
				windows[i]->SetModelMatrix(1);
				break;
			}
		}
	}

	//if (key == GLFW_KEY_C && action == GLFW_PRESS) {
	//	if (previousWindow != NULL) {
	//		My_window* p = previousWindow;
	//		My_window* c = currentWindow;
	//		m_newWindow();
	//		windows.back()->texture = new float[c->textureWidth * c->textureHeight * 3];
	//		m_combinePictures(c->texture, p->texture, windows.back()->texture, 0.5f, c->textureWidth, c->textureHeight);
	//		windows.back()->setTexture(c->textureWidth, c->textureHeight, windows.back()->texture);
	//	}
	//}

	//if (key == GLFW_KEY_P && action == GLFW_PRESS) {
	//	for (int i = 0; i < windows.size(); ++i) {
	//		if (windows[i]->window == window) {
	//			int wi = windows[i]->textureWidth;
	//			int he = windows[i]->textureHeight;
	//			if (windows[i]->texture == NULL) {
	//				wi = 700;
	//				he = 700;
	//				windows[i]->texture = new float[wi*he * 3];
	//			}
	//			PerlinNoise pn(wi, he, 6);
	//			pn.m_genPerlinNoise(windows[i]->texture, 3);
	//			windows[i]->setTexture(wi, he, windows[i]->texture);
	//			break;
	//		}
	//	}
	//}

	//if (key == GLFW_KEY_O && action == GLFW_PRESS) {
	//	for (int i = 0; i < windows.size(); ++i) {
	//		if (windows[i]->window == window) {
	//			int wi = windows[i]->textureWidth;
	//			int he = windows[i]->textureHeight;
	//			if (windows[i]->texture == NULL) {
	//				wi = 700;
	//				he = 700;
	//				windows[i]->texture = new float[wi*he * 3];
	//			}
	//			WorleyNoise pn(wi, he, 8, 6);
	//			pn.m_genWorleyNoise(windows[i]->texture, 3);
	//			windows[i]->setTexture(wi, he, windows[i]->texture);
	//			break;
	//		}
	//	}
	//}

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
#pragma endregion


Object3D RayHitObject(My_window* w) {
	vec3 rayOrigin = w->worldSpaceCameraPos;
	vec3 rayTarget = w->worldSpaceCameraTarget;
	glfwGetCursorPos(currentWindow->window, &cursor_xpos, &cursor_ypos);
	float rayDisplace = 0.5;
	rayTarget.x += rayDisplace * ((cursor_xpos / w->windowPixelSizeX) - 0.5) * 2;
	rayTarget.y += rayDisplace * ((cursor_ypos / w->windowPixelSizeY) - 0.5) * 2;
	/*rayOrigin.x += rayTarget.x;
	rayOrigin.y += rayTarget.y;*/
	std::cout << rayTarget.x <<" "<< rayTarget.y << "\n";
	std::vector<Object3D*> hits;
	for each (Object3D* o in w->obj3D)
	{
		for each  (Triangle* tri in o->bbtris)
		{
			//bounding box scalee v‰‰rin (skaalaantuu 0,0 mukaan)
			float A = tri->normal.x;
			float B = tri->normal.y;
			float C = tri->normal.z;
			vec3 normal = vec3(A, B, C);
			mat4 model = w->Models.back();
			vec3 vert = w->View * model * vec4(tri->corners[0].x, tri->corners[0].y, tri->corners[0].z,1);
			//kattoo ettei oo s‰teen kans saman suuntaiset
			if (dot(normal, rayTarget) != 0){
				//distance
				float dist = dot(normal, vert-rayOrigin) / dot(normal, rayTarget-rayOrigin);
				vec3 hit = rayOrigin + (dist * (rayTarget-rayOrigin));
				//is hit inside triangle
				vec3 first = model * vec4(tri->corners[0].x, tri->corners[0].y, tri->corners[0].z, 1);
				vec3 second = model * vec4(tri->corners[1].x, tri->corners[1].y, tri->corners[1].z, 1);
				vec3 third = model * vec4(tri->corners[2].x, tri->corners[2].y, tri->corners[2].z, 1);
				if (dot(cross(second - first, hit - first), normal) >= 0 && dot(cross(third - second, hit - second), normal) >= 0 && dot(cross(first - third, hit - third), normal) >= 0) {
					std::cout << "osuin\n";
				}
			}
		}
	}
	Object3D k;
	return k;
}

Object3D* m_genCube(glm::vec3 minCoord, glm::vec3 maxCoord) { //UV:t puuttuu
	Object3D* obj = new Object3D();
	bool values = false;
	float tx, ty, tz;
	for (int z = 0; z <= 1; ++z) {
		tz = minCoord.z;
		for (int y = 0; y <= 1; ++y) {
			ty = minCoord.y;
			for (int x = 0; x <= 1; ++x) {
				tx = minCoord.x;
				if (x == 1) {
					tx = maxCoord.x;
				}
				if (y == 1) {
					ty = maxCoord.y;
				}
				if (z == 1) {
					tz = maxCoord.z;
				}
				obj->verts.push_back(new Vertex(obj->verts.size(), tx, ty, tz));
			}
		}
	}
	//counterclockvice
	obj->tris.push_back(new Triangle(*obj->verts[0], *obj->verts[1], *obj->verts[3]));
	obj->tris.push_back(new Triangle(*obj->verts[0], *obj->verts[4], *obj->verts[1]));
	obj->tris.push_back(new Triangle(*obj->verts[0], *obj->verts[3], *obj->verts[2]));
	obj->tris.push_back(new Triangle(*obj->verts[0], *obj->verts[2], *obj->verts[4]));
	obj->tris.push_back(new Triangle(*obj->verts[1], *obj->verts[7], *obj->verts[3]));
	obj->tris.push_back(new Triangle(*obj->verts[1], *obj->verts[4], *obj->verts[5]));
	obj->tris.push_back(new Triangle(*obj->verts[1], *obj->verts[5], *obj->verts[7]));
	obj->tris.push_back(new Triangle(*obj->verts[2], *obj->verts[3], *obj->verts[7]));
	obj->tris.push_back(new Triangle(*obj->verts[2], *obj->verts[6], *obj->verts[4]));
	obj->tris.push_back(new Triangle(*obj->verts[2], *obj->verts[7], *obj->verts[6]));
	obj->tris.push_back(new Triangle(*obj->verts[4], *obj->verts[6], *obj->verts[5]));
	obj->tris.push_back(new Triangle(*obj->verts[5], *obj->verts[6], *obj->verts[7]));

	return obj;
}

Object3D* m_genPlane(int x, int y) {
	Object3D* obj = new Object3D();
	int size = x  * y;
	int index = -1;
	int index2 = -1;
	for (int i = 0; i < y ; ++i) {
		for (int j = 0; j < x; ++j) {
			Vertex* v = obj->AddVertex((float)j/ (float)(x-1), (float)i/ (float)(x-1), 0);
			v->u = (float)j/ (float)(x -1);
			v->v = (float)i/ (float)(x-1);
		}
	}
	//varmista et menee vastap‰iv‰‰n kolmion vertexit (ne n‰ytt‰‰ kyl menev‰n oikein)
	index = -1;
	for (int i = 0; i < y - 1; ++i) {
		for (int j = 0; j < x - 1; ++j) {
			Triangle* tri = new Triangle(*obj->verts[(j + i * (y))], *obj->verts[(j + 1 + (i + 1) * (y))], *obj->verts[(j + 1 + i * (y))]);
			obj->tris.push_back(tri);
			tri = new Triangle(*obj->verts[(j + i * (y))], *obj->verts[(j + (i + 1) * (y))], *obj->verts[(j + 1 + (i + 1) * (y))]);
			obj->tris.push_back(tri);
		}
	}
	return obj;
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
	//My_window* w2 = new My_window("window2");
	windows.push_back(window);
	//windows.push_back(w2);
	glfwMakeContextCurrent(window->window);
	currentWindow = window;
	Object3D* o = m_genPlane(5, 5);
	//o->Scale(&float3(0.5, 0.5, 1));
	o->positionFactor = &float3(0,0,1);
	o->BuildBoundingBox();
	currentWindow->AddObject3D(o);
	//currentWindow->AddObject3D(m_genPlane(5, 5));

	const int wi = 700, he = 700;

	//PerlinNoise pn(wi, he, 6);
	//WorleyNoise wn(wi, he, 8, 5);

	//wn.m_genWorleyNoise(pic2, 4);
	//clock_t begin = clock();
	//pn.m_genPerlinNoise(pic,3);
	//clock_t end = clock();
	//std::cout << double(end - begin) / CLOCKS_PER_SEC << "\n";

	//FractalLine f(200, 200, 500, 200);
	//f.addLine(500, 500);
	//f.addLine(200, 500);
	//int2 coor[] = { int2(200,500) };
	//f.addLines(coor, 1);
	//f.closeLines();
	//for (int r = 0; r < 3; ++r) {
	//	//std::cout << "\nITERATE START!\n\n";
	//	f.iterFractal();
	//}
	////f.printPoints();
	//m_drawFractal(f, pic, wi, he);
	//m_drawLine(f.c0.x, f.c0.y, f.c1.x, f.c1.y, pic, wi, he);
	//m_saveAsPNG("kuvaggg.png", wi, he, pic, "k");
	Texture tex = Texture(wi, he);
	tex.GenRandomNoiseColor();
	currentWindow->setTexture(*o, tex);
	//w2->setTexture(wi, he, pic2);

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