#define GLEW_STATIC


#include <stdio.h>
#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
//#include <glm\glm.hpp>
//#include <glm\gtc\matrix_transform.hpp>

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
		glfwTerminate();
		return NULL;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	return window;
}

int main() {
	GLFWwindow* window = InitWindow();
	GLuint moi;
	std::cout << "moi";
	for (int i = 0; i < 2000; ++i) {
		std::cout << i << "\n";
	}
}

