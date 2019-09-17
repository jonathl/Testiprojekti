#pragma once
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

struct UVector
{
	glm::vec3 uvec;
	int posx;
	int posy;
	int posz = 0;
};

struct vectorGrid 
{
	UVector* v[4];
};

class PerlinNoise {
private:
	int width;
	int height;
	int gridsize; //size in pixels of grid
	int grids; //grids x grids = grid
	UVector* unitv; //vectors in grid

public:
	//PerlinNoise() {};
	PerlinNoise(int wi, int he, int g);
	UVector GenRandomUnitVector();
	void OctavePerlin(float* texture);
	void GenPerlinNoise1D(const int width, float* texture);
	void GenPerlinNoise(float* texture, int octave);
};

class WorleyNoise {
private:
	int width;
	int height;
	int gridsize; //size in pixels of grid
	int grids; //grids x grids = grid
	float range;

public:
	WorleyNoise(int wi, int he, int g, float r);
	void GenWorleyNoise(float* texture, int octave);
};

class FractalLine { //k‰ytt‰‰ textuurin pisteit‰ (mink‰ takia oli ennen int2)
private:
public:
	glm::vec3 c0;
	glm::vec3 c1;
	std::vector<FractalLine*> fl;

	FractalLine();
	FractalLine(int tx, int ty, int tx1, int ty1);
	FractalLine(glm::vec3 tc0, glm::vec3 tc1);
	void Set(glm::vec3 tc0, glm::vec3 tc1);
	void AddLine(int x, int y);
	void AddLines(glm::vec3* coord, int amount);
	void CloseLines();
	void PrintPoints(); //toString
	void IterFractal();
};

class FractalTriangle {
private:
public:
	FractalTriangle(){}
};