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
#include <vector>
#include <time.h>
#include <ctime>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

using namespace glm;

double cursor_xpos, cursor_ypos;
bool cursorLeftPressed = false;

struct UVector
{
	float x;
	float y;
	int posx;
	int posy;
};

struct vectorGrid 
{
	UVector *v0, *v1, *v2, *v3;
};

int m_getTextureCoordinate(int width, int height) {
	int textcoord = width * (height-cursor_ypos) + cursor_xpos;
	//std::cout << "texcoord = " << textcoord << "\n";
	return textcoord;
}

void m_drawToTexture(GLuint tex,float *texture, int width, int height) { //korjaa reunan yli piirtäminen ja textuurin ko-on vaikuttaminen
	float color[] = { 1.0f, 0.0f, 0.0f };
	int index = m_getTextureCoordinate(width, height) * 3;
	texture[index] = color[0];
	texture[++index] = color[1];
	texture[++index] = color[2];
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, texture);
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
	window = glfwCreateWindow(512, 512, "Testiprojekti", NULL, NULL);
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

UVector m_genRandomUnitVector() {
	int angle = rand() % 359;
	UVector u;
	u.x = cos(float(angle));
	u.y = sin(float(angle));
	return u;
}

//void m_genVectors(UVector v) {
//	for (int i = 0; i < 5; ++i) {
//		for (int j = 0; j < 5; ++j) {
//			v[i][j] = m_genRandomUnitVector();
//		}
//	}
//}

void m_genWhiteTex(int width, int height, float *texture) {
	int index = -1;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			texture[++index] = 1.0f;
			texture[++index] = 1.0f;
			texture[++index] = 1.0f;
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

void m_genRandomNoise(int width, int height, float *texture) {
	int index = -1;
	float r_value;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			r_value = float(rand() % 100)/100;
			texture[++index] = r_value;
			texture[++index] = r_value;
			texture[++index] = r_value;
		}
	}
}

void m_genPerlinNoise(const int width,const int height, float *texture) { //kokeile yhdellä ruudulla
	const int gridsize = 15;
	const int grids = 2;
	std::vector<std::vector<float>> perl;
	UVector unitv[grids+1][grids+1];
	for (int i = 0; i < grids +1; ++i) {  //generate unit vectors
		for (int j = 0; j < grids +1; ++j) {
			unitv[i][j] = m_genRandomUnitVector();
			unitv[i][j].posx = gridsize*i;
			unitv[i][j].posy = gridsize*j;
		}
	}
	unitv[1][1].x = 1.0f; //poista
	unitv[1][1].y = 0.0f;
	vectorGrid vg[grids*grids]; //generate vector grid
	int index = 0;
	for (int i = 0; i < grids; ++i) {  
		for (int j = 0; j < grids; ++j) {
			vg[index].v0 = &unitv[j][i];
			vg[index].v1 = &unitv[j+1][i];
			vg[index].v2 = &unitv[j][i+1];
			vg[index].v3 = &unitv[j+1][i+1];
			++index;
		}
	}
	index = -1;
	int gi = -1; //gridindex
	float r_value = 1.0f;
	vectorGrid vgi;
	for (int i = 0; i < height; ++i) {
		if (i % gridsize == 0) {
			gi += grids;
		}
		gi -= grids -1;
		for (int j = 0; j < width; ++j) {
			if (j != 0 && j % gridsize == 0) {
				++gi;
			} 
			if (gi == 3) {
				gi = 3;
			}
			float d0x = float(j - vg[gi].v0->posx);
			float d0y = float(i - vg[gi].v0->posy);
			float d0l =sqrt(d0x*d0x+ d0y*d0y);
			d0x /= d0l;
			d0y /= d0l;
			float d1x = float(j - vg[gi].v1->posx);
			float d1y = float(i - vg[gi].v1->posy);
			float d1l = sqrt(d1x*d1x + d1y*d1y);
			d1x /= d1l;
			d1y /= d1l;
			float d2x = float(j - vg[gi].v2->posx);
			float d2y = float(i - vg[gi].v2->posy);
			float d2l = sqrt(d2x*d2x + d2y*d2y);
			d2x /= d2l;
			d2y /= d2l;
			float d3x = float(j - vg[gi].v3->posx);
			float d3y = float(i - vg[gi].v3->posy);
			float d3l = sqrt(d3x*d3x + d3y*d3y);
			d3x /= d3l;
			d3y /= d3l;
			float d0 = vg[gi].v0->x * d0x + vg[gi].v0->y * d0y;
			float d1 = vg[gi].v1->x * d1x + vg[gi].v1->y * d1y;
			float d2 = vg[gi].v2->x * d2x + vg[gi].v2->y * d2y;
			float d3 = vg[gi].v3->x * d3x + vg[gi].v3->y * d3y;


			float di0 = d0 + 0.5f * (d1 - d0);
			float di1 = d2 + 0.5f * (d3 - d2);
			r_value = di0 + 0.5f * (di1 - di0);
			
			
			if (gi == 0) {
				r_value = (d3 + 1) / 2;
			}
			else if (gi == 1) {
				r_value = (d2 + 1) / 2;
			}
			else if (gi == 2) {
				r_value = (d1 + 1) / 2;
			}
			else {
				r_value = (d0 + 1) / 2;
			}
			texture[++index] = r_value;
			texture[++index] = r_value;
			texture[++index] = r_value;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		std::cout << "moi";
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		cursorLeftPressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		cursorLeftPressed = false;
	}
}

int main()
{
	srand(time(NULL));
	//m_genRandomUnitVector();
	GLFWwindow* window = InitWindow();
	glClearColor(0.1f, 0.5f, 0.7f, 1.0f);

	const int wi = 30, he = 30;

	float* pic = new float[wi*he * 3];

	m_genPerlinNoise(wi, he, pic);

	float vertices[] = {
		//  Position      Color             Texcoords
		-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Top-left
		1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Top-right
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-right
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f  // Bottom-left
	};

	unsigned int indices[] = {
		0, 1, 2, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wi, he, 0, GL_RGB, GL_FLOAT, pic);

	GLuint programID = LoadShaders("VertexShader.vertexshader", "FragmentShader.fragmentshader");

	glfwSetMouseButtonCallback(window, mouse_button_callback);

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);

		glBindTexture(GL_TEXTURE_2D, tex);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
		if (cursorLeftPressed) {
			glfwGetCursorPos(window, &cursor_xpos, &cursor_ypos);
			m_drawToTexture(tex, pic, wi, he);
		}
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwTerminate();
}