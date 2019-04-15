#pragma once

class Object3D{
private:
	float* vertices;
	float* normals;
	float* texcoord;
	unsigned int* indices;

public:
	Object3D() {

	}
	void setVertices(float *v) {
		vertices = v;
	}
};