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
	float3 normal;
	Triangle(){}
	Triangle(Vertex first, Vertex second, Vertex third) {
		corners = new Vertex[3];
		corners[0] = first;
		corners[1] = second;
		corners[2] = third;
		float3 f = float3(second.x - first.x, second.y - first.y, second.z - first.z);
		float3 s = float3(third.x - first.x, third.y - first.y, third.z - first.z);
		normal.x = f.y * s.z - f.z * s.y;
		normal.y = f.z * s.x - f.x * s.z;
		normal.z = f.x * s.y - f.y * s.x;
		std::cout << normal.x << " " << normal.y << " " << normal.z << "\n";
	}

};

class Object3D{
private:


public:

	float* texture;
	int textureWidth;
	int textureHeight;

	int verticesSize = 0;
	float3* pivotPoint = new float3(0.5, 0.5, 0);
	float3* positionFactor = new float3(0.0, 0.0, 0.0);
	float3* rotateFactor = new float3(0.0, 0.0, 0.0);
	float3* scaleFactor = new float3(1.0, 1.0, 1.0);

	std::vector<Vertex*> verts;
	std::vector<Triangle*> tris;
	//bounding box
	std::vector<Vertex*> bbverts;  
	std::vector<Triangle*> bbtris;
	Object3D() {

	}
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
		rotateFactor->z = z;
		rotateFactor->y = y;
		rotateFactor->x = x;
	}
	void AddRotate(float x, float y, float z) {
		rotateFactor->z += z;
		rotateFactor->y += y;
		rotateFactor->x += x;
	}
	void Move(float x, float y, float z) {
		positionFactor->z += z;
		positionFactor->y += y;
		positionFactor->x += x;
	}
	void SetTexture(int wi, int he, float* pic) {
		texture = pic;
		textureWidth = wi;
		textureHeight = he;
	}
	void BuildBoundingBox() {
		float minX = NULL;
		float minY= 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
		bool values = false;
		for each (Vertex* v in verts)
		{
			//initialize values
			if (values == false) {
				minX = v->x;
				minY = v->y;
				minZ = v->z;
				maxX = v->x;
				maxX = v->y;
				maxX = v->z;
				values = true;
			}
			else {
				if (minX > v->x) {
					minX = v->x;
				}
				else if (maxX < v->x) {
					maxX = v->x;
				}
				if (minY > v->y) {
					minY = v->y;
				}
				else if (maxY < v->y) {
					maxY = v->y;
				}
				if (minZ > v->z) {
					minZ = v->z;
				}
				else if (maxZ < v->z) {
					maxZ = v->z;
				}

			}
		}
		float tx, ty, tz;
		for (int z = 0; z <= 1; ++z) {
			tz = minZ;
			for (int y = 0; y <= 1; ++y) {
				ty = minY;
				for (int x = 0; x <= 1; ++x) {
					tx = minX;
					if (x == 1) {
						tx = maxX;
					}
					if (y == 1) {
						ty = maxY;
					}
					if (z == 1) {
						tz = maxZ;
					}
					bbverts.push_back(new Vertex(bbverts.size(), tx, ty, tz));
				}
			}
		}
		//counterclockvice
		bbtris.push_back(new Triangle(*bbverts[0], *bbverts[1], *bbverts[3]));
		bbtris.push_back(new Triangle(*bbverts[0], *bbverts[4], *bbverts[1]));
		bbtris.push_back(new Triangle(*bbverts[0], *bbverts[3], *bbverts[2]));
		bbtris.push_back(new Triangle(*bbverts[0], *bbverts[2], *bbverts[4]));
		bbtris.push_back(new Triangle(*bbverts[1], *bbverts[7], *bbverts[3]));
		bbtris.push_back(new Triangle(*bbverts[1], *bbverts[4], *bbverts[5]));
		bbtris.push_back(new Triangle(*bbverts[1], *bbverts[5], *bbverts[7]));
		bbtris.push_back(new Triangle(*bbverts[2], *bbverts[3], *bbverts[7]));
		bbtris.push_back(new Triangle(*bbverts[2], *bbverts[6], *bbverts[4]));
		bbtris.push_back(new Triangle(*bbverts[2], *bbverts[7], *bbverts[6]));
		bbtris.push_back(new Triangle(*bbverts[4], *bbverts[6], *bbverts[5]));
		bbtris.push_back(new Triangle(*bbverts[5], *bbverts[6], *bbverts[7]));
	}
	void FindCenter() {

	}
};

