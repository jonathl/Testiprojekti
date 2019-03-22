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
#include <vector>
#include <time.h>
#include <ctime>
#include <malloc.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Noise.h"

#include <png.h>
#include <zlib.h>


using namespace glm;

double cursor_xpos, cursor_ypos;
bool cursorLeftPressed = false;

//struct UVector
//{
//	float x;
//	float y;
//	int posx;
//	int posy;
//};
//
//struct vectorGrid 
//{
//	UVector* v[4];
//};

int m_getTextureCoordinate(int width, int height, int x, int y) {
	int textcoord = width * (height-y) + x;
	//std::cout << "texcoord = " << textcoord << "\n";
	return textcoord;
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
	texture[index2] = 0.3f;
	texture[index2+ 1] = 0.2f;
	texture[index2 + 2] = 1.0f;
	index2 = m_getTextureCoordinate(width, height, x0, y0) * 3;
	texture[index2] = 0.3f;
	texture[index2 + 1] = 0.2f;
	texture[index2 + 2] = 1.0f;


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

int main()
{
	srand(time(NULL));
	GLFWwindow* window = InitWindow();
	glClearColor(0.1f, 0.5f, 0.7f, 1.0f);

	const int wi = 700, he = 700;

	float* pic = new float[wi*he * 3];

	/*WorleyNoise pn(wi, he, 4, 4.0f);
	clock_t begin = clock();
	pn.m_genWorleyNoise(pic,3);
	clock_t end = clock();
	std::cout << double(end - begin) / CLOCKS_PER_SEC << "\n";*/
	m_genOneColorTex(wi, he, pic, 0.7f, 0.7f, 0.9f);
	//m_drawGridOnTex(wi, he, 125, pic);
	FractalLine f(200,100,600,100);
	f.addLine(600, 500);
	int2 coor[] = { int2(200,500) };
	f.addLines(coor, 1);
	f.closeLines();
	for (int r = 0; r < 5; ++r) { //stackki täyttyy jos yli 5(?)
		//std::cout << "\nITERATE START!\n\n";
		f.iterFractal();
	}
	//f.printPoints();
	m_drawFractal(f, pic, wi, he);
	//m_drawLine(f.c0.x, f.c0.y, f.c1.x, f.c1.y, pic, wi, he);
	m_saveAsPNG("kuva.png", wi, he, pic, "k");

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