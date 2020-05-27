#include "3D_object.h"

Object3D::Object3D() {

}
Vertex* Object3D::AddVertex(float x, float y, float z) {
	Vertex* ver = new Vertex(verts.size(), x, y, z);
	verts.push_back(ver);
	++verticesSize;
	return ver;
}
Vertex* Object3D::AddVertex(glm::vec3 position) {
	Vertex* ver = new Vertex(verts.size(), position);
	verts.push_back(ver);
	++verticesSize;
	return ver;
}
void Object3D::Scale(glm::vec3 newScale) {
	scaleFactor->x = newScale.x;
	scaleFactor->y = newScale.y;
	scaleFactor->z = newScale.z;
}
void Object3D::AddScale(glm::vec3 newScale) {
	scaleFactor->x += newScale.x;
	scaleFactor->y += newScale.y;
	scaleFactor->z += newScale.z;
}
void Object3D::DecScale(glm::vec3 newScale) {
	scaleFactor->x -= newScale.x;
	scaleFactor->y -= newScale.y;
	scaleFactor->z -= newScale.z;
}
void Object3D::Rotate(float x, float y, float z) {
	rotateFactor->z = z;
	rotateFactor->y = y;
	rotateFactor->x = x;
}
void Object3D::AddRotate(float x, float y, float z) {
	rotateFactor->z += z;
	rotateFactor->y += y;
	rotateFactor->x += x;
}
void Object3D::Move(float x, float y, float z) {
	positionFactor->z += z;
	positionFactor->y += y;
	positionFactor->x += x;
}
void Object3D::SetTexture(Texture* tex) {
	texture = tex;
}
void Object3D::BuildBoundingBox() { //min and max values to vec3
	float minX = NULL;
	float minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	bool values = false;
	for each (Vertex * v in verts)
	{
		//initialize values
		if (values == false) {
			minX = v->pos.x;
			minY = v->pos.y;
			minZ = v->pos.z;
			maxX = v->pos.x;
			maxX = v->pos.y;
			maxX = v->pos.z;
			values = true;
		}
		else {
			if (minX > v->pos.x) {
				minX = v->pos.x;
			}
			else if (maxX < v->pos.x) {
				maxX = v->pos.x;
			}
			if (minY > v->pos.y) {
				minY = v->pos.y;
			}
			else if (maxY < v->pos.y) {
				maxY = v->pos.y;
			}
			if (minZ > v->pos.z) {
				minZ = v->pos.z;
			}
			else if (maxZ < v->pos.z) {
				maxZ = v->pos.z;
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
void Object3D::FindCenter() {
	glm::vec3 center(0, 0, 0);
	for (int i = 0; i < verticesSize; ++i) {
		center += verts[i]->pos;
	}
	center /= verticesSize;
	*pivotPoint = center;
}

void Object3D::UVWfromLoc()
{
	for (int i = 0; i < verticesSize; ++i) {
		Vertex* v = verts[i];
		v->uvw = v->pos;
	}
}
