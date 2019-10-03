#pragma once
#pragma warning(disable:4996)

#include <math.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Noise.h"

#include <png.h>
#include <zlib.h>

class Texture {
public:
	unsigned int width;
	unsigned int height;
	unsigned int texelCount;
	float* pic;

private:
	float ChangeScale(float oldmin, float oldmax, float newmin, float newmax, float value);
	void write_row_callback(png_structp png_ptr, png_uint_32 row, int pass) {
		//std::cout << row <<"\n";
	}
public:
	Texture(int wi, int he);
	int GetTextureCoordinate(int x, int y);
	void DrawToTexture(int x, int y);
	//kesken
	//void colorAdjacent(float* color, float* fillcolor, int texel, float* texture, int width, int height); //poista recursio, stackki täyttyy
	//void bucketToolTexture(int x, int y);
	//void bucketToolTexture2(int x, int y);
	//
	void GenOneColorTex(float r, float g, float b);
	void GenOneColorTex(float r, float g, float b, float a);
	void GenCheckerboardTex();
	void DrawGridOnTex(int gridsize);
	void DrawGridOnTex(int gridsize, float r, float g, float b);
	void DrawLine(int x0, int y0, int x1, int y1);
	void DrawLine(glm::vec3 c0, glm::vec3 c1, bool texcoord = false);
	void DrawFractal(FractalLine f);
	void CombinePictures(Texture texture, Texture* rtex, float str);
	void AddContrast(float str, float displacement);
	void GenRandomNoise();
	void GenRandomNoiseColor();
	void GenPerlinNoise();
	void GenWorleyNoise();
	void SaveAsPNG(char* file_name, char* title);
};

class Texture3D {
public:
	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int texelCount;
	float* pic;

	Texture3D(int wi, int he, int de);
	int GetTextureCoordinate(int x, int y, int z);
	void GenCheckerboardTex3D();
	void GenRandomNoise();
	void GenRandomNoiseColor();
	void GenPerlinNoise();
	void GenWorleyNoise();
};