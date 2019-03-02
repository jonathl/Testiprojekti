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
	UVector* v[4];
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
	const int gridsize = 125;
	const int grids = 4;
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
			vg[index].v[0] = &unitv[j][i];
			vg[index].v[1] = &unitv[j+1][i];
			vg[index].v[2] = &unitv[j][i+1];
			vg[index].v[3] = &unitv[j+1][i+1];
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
			float dx[4];
			float dy[4];
			float dl[4];
			for (int k = 0; k < 4; ++k) {
				dx[k] = float(j - vg[gi].v[k]->posx);
				dy[k] = float(i - vg[gi].v[k]->posy);
				dl[k] = sqrt(abs(dx[k]*dx[k] + dy[k]*dy[k]));
				if (dl[k] > 0) {
					dx[k] /= dl[k];
					dy[k] /= dl[k];
				}
				else {
					dx[k] = vg[gi].v[k]->x;
					dx[k] = vg[gi].v[k]->y;
				}
			}
			float d0 = vg[gi].v[0]->x * dx[0] + vg[gi].v[0]->y * dy[0];
			float d1 = vg[gi].v[1]->x * dx[1] + vg[gi].v[1]->y * dy[1];
			float d2 = vg[gi].v[2]->x * dx[2] + vg[gi].v[2]->y * dy[2];
			float d3 = vg[gi].v[3]->x * dx[3] + vg[gi].v[3]->y * dy[3];
			/*d0 = (d0 + 1) / 2;
			d1 = (d1 + 1) / 2;
			d2 = (d2 + 1) / 2;
			d3 = (d3 + 1) / 2;*/

			float t = float(j%gridsize) / gridsize;
			float jl = t * t * t * (t * (t * 6 - 15) + 10);
			t = float(i%gridsize) / gridsize;
			float jl0 = t * t * t * (t * (t * 6 - 15) + 10);
			
			/*float jl = (dx[0] + 1)/2;
			float jl2 = (dx[2] + 1) / 2;
			float jl3 = (dy[0] + 1) / 2;*/
			/*if (vg[gi].v[0]->posx % gridsize == 0) {
				jl = (dx[1] + 1) / 2;
			}
			if (vg[gi].v[2]->posx % gridsize == 0) {
				jl2 = (dx[3] + 1) / 2;
			}
			if (vg[gi].v[0]->posy % gridsize == 0) {
				jl = (dy[1] + 1) / 2;
			}*/
			float di0 = d0 + jl * (d1 - d0);
			float di1 = d2 + jl * (d3 - d2);
			r_value = di0 + jl0 * (di1 - di0);
			
			
			/*if (gi == 0) {
				r_value = d3;
			}
			else if (gi == 1) {
				r_value = d2;
			}
			else if (gi == 2) {
				r_value = d1;
			}
			else {
				r_value = d0;
			}*/
			r_value = di0 + jl0 * (di1 - di0);
			r_value = (r_value + 1) / 2;
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

	const int wi = 500, he = 500;

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