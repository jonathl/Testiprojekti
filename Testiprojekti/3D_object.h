#pragma once
#include <vector>


class float3 {
public:
	float x;
	float y;
	float z;
	float3() {}
	float3(float x0, float y0, float z0) {
		x = x0;
		y = y0;
		z = z0;
	}
	void set(float x0, float y0, float z0) {
		x = x0;
		y = y0;
		z = z0;
	}
	float3 float3::operator+(const float3& other) {
		return float3(x + other.x, y + other.y, z + other.z);
	}
};

class Vertex {
private:

public:
	unsigned int index;
	float x;
	float y;
	float z;
	float u;
	float v;

	Vertex() {

	}

	Vertex(unsigned int ni, float nx, float ny, float nz) {
		index = ni;
		x = nx;
		y = ny;
		z = nz;
	}
};

class Triangle {
private:

public:
	Vertex* corners;
	Triangle(){}
	Triangle(Vertex first, Vertex second, Vertex third) {
		corners = new Vertex[3];
		corners[0] = first;
		corners[1] = second;
		corners[2] = third;
	}

};

class Object3D{
private:


public:
	int verticesSize = 0;
	float3* pivotPoint = new float3(0.5, 0.5, 0);
	float3* positionFactor = new float3(0.0, 0.0, 0.0);
	float3* rotateFactor = new float3(0.0, 0.0, 80.0);
	float3* scaleFactor = new float3(1.0, 1.0, 1.0);
	/*float* vertices;
	float* normals;
	float* texcoord;
	unsigned int* indices;*/
	std::vector<Vertex*> verts;
	std::vector<Triangle*> tris;
	Object3D() {

	}
	/*void setVertices(float *v) {
		vertices = v;
	}*/
	Vertex* AddVertex(float x, float y, float z) {
		Vertex* ver = new Vertex(verts.size(), x, y, z);
		verts.push_back(ver);
		++verticesSize;
		return ver;
	}
	void Scale(float3* newScale) {
		scaleFactor = newScale;
	}
	void AddScale(float3 newScale) {
		scaleFactor->x += newScale.x;
		scaleFactor->y += newScale.y;
		scaleFactor->z += newScale.z;
	}
	void DecScale(float3 newScale) {
		scaleFactor->x -= newScale.x;
		scaleFactor->y -= newScale.y;
		scaleFactor->z -= newScale.z;
	}
	void Rotate(float x, float y, float z) {
		rotateFactor->z += z;
	}
	void FindCenter() {

	}
};

