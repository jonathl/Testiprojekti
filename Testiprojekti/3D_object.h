#pragma once
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Texture.h" 




class Vertex {
private:

public:
	unsigned int index;
	glm::vec3 pos;
	glm::vec3 uvw;

	Vertex() {

	}

	Vertex(unsigned int ni, float nx, float ny, float nz) {
		index = ni;
		pos.x = nx;
		pos.y = ny;
		pos.z = nz;
	}

	Vertex(unsigned int ni, glm::vec3 position) {
		index = ni;
		pos = position;
	}
};

class Triangle {
private:

public:
	Vertex* corners;
	glm::vec3 normal;
	Triangle(){}
	Triangle(Vertex first, Vertex second, Vertex third) {
		corners = new Vertex[3];
		corners[0] = first;
		corners[1] = second;
		corners[2] = third;
		glm::vec3 f = glm::vec3(second.pos.x - first.pos.x, second.pos.y - first.pos.y, second.pos.z - first.pos.z);
		glm::vec3 s = glm::vec3(third.pos.x - first.pos.x, third.pos.y - first.pos.y, third.pos.z - first.pos.z);
		normal.x = f.y * s.z - f.z * s.y;
		normal.y = f.z * s.x - f.x * s.z;
		normal.z = f.x * s.y - f.y * s.x;
		//std::cout << normal.x << " " << normal.y << " " << normal.z << "\n";
	}

};

class Object3D{
private:


public:

	Texture* texture;

	int verticesSize = 0;
	glm::vec3* pivotPoint = new glm::vec3(0.5, 0.5, 0);
	glm::vec3* positionFactor = new glm::vec3(0.0, 0.0, 0.0);
	glm::vec3* rotateFactor = new glm::vec3(0.0, 0.0, 0.0);
	glm::vec3* scaleFactor = new glm::vec3(1.0, 1.0, 1.0);

	std::vector<Vertex*> verts;
	std::vector<Triangle*> tris;
	//bounding box
	std::vector<Vertex*> bbverts;  
	std::vector<Triangle*> bbtris;

	Object3D();
	Vertex* AddVertex(float x, float y, float z);
	Vertex* AddVertex(glm::vec3 position);
	void Scale(glm::vec3 newScale);
	void AddScale(glm::vec3 newScale);
	void DecScale(glm::vec3 newScale);
	void Rotate(float x, float y, float z);
	void AddRotate(float x, float y, float z);
	void Move(float x, float y, float z);
	void SetTexture(Texture* tex);
	void BuildBoundingBox();
	void FindCenter();
	void UVWtoLoc();
};

