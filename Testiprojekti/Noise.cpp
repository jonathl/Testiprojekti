#include "Noise.h"

//PerlinNoise

PerlinNoise::PerlinNoise(unsigned int wi, unsigned int he, unsigned int de, unsigned int gx, unsigned int gy, unsigned int gz) {
	width = wi;
	height = he;
	depth = de;
	if (gy == 0) {
		gridDimensions = glm::ivec3(gx);
	}
	else {
		gridDimensions = glm::ivec3(gx, gy, gz);
	}
	if (de == 0)
	{
		gridPixelDimensions = glm::ivec3(wi / gridDimensions.x + 1, he / gridDimensions.y + 1, 0); // + 1 is to avoid parts with no noise caused by rounding errors (?)
		int vectorsize = (gridDimensions.x + 1) * (gridDimensions.y + 1);
		unitv = new UVector[vectorsize];
		for (int i = 0; i < vectorsize; ++i) {  //generate unit vectors
			unitv[i] = GenRandomUnitVector();
		}
	}
	else {
		gridPixelDimensions = glm::ivec3(wi / gridDimensions.x + 1, he / gridDimensions.y + 1, de / gridDimensions.z + 1);
		int vectorsize = (gridDimensions.x + 1) * (gridDimensions.y + 1) * (gridDimensions.z + 1);
		unitv = new UVector[vectorsize];
		for (int i = 0; i < vectorsize; ++i) {  //generate unit vectors
			unitv[i] = GenRandomUnitVector3D();
		}
	}
	
}

//PerlinNoise::PerlinNoise(int wi, int he, int g) {
//	width = wi;
//	height = he;
//	depth = 0;
//	gridDimensions = glm::ivec3(g); // pow(2, g); //muuta? 
//	gridPixelDimensions = glm::ivec3(wi / gridDimensions.x + 1, he / gridDimensions.y +1, 0); // + 1 is to avoid parts with no noise caused by rounding errors (?)
//	int vectorsize = (gridDimensions.x + 1) * (gridDimensions.y + 1); // vertices
//	unitv = new UVector[vectorsize];
//	for (int i = 0; i < vectorsize; ++i) {  //generate unit vectors
//		unitv[i] = GenRandomUnitVector();
//	}
//}
//
//PerlinNoise::PerlinNoise(int wi, int he, int de, int g)
//{
//	width = wi;
//	height = he;
//	depth = de;
//	gridDimensions = glm::ivec3(g); // pow(2, g); //muuta? 
//	gridPixelDimensions = glm::ivec3(wi / gridDimensions.x + 1, he / gridDimensions.y +1, de / gridDimensions.z +1);
//	int vectorsize = (gridDimensions.x + 1) * (gridDimensions.y + 1) * (gridDimensions.z + 1); 
//	unitv = new UVector[vectorsize];
//	for (int i = 0; i < vectorsize; ++i) {  //generate unit vectors
//		unitv[i] = GenRandomUnitVector3D();
//	}
//}

UVector PerlinNoise::GenRandomUnitVector() {
	int angle = rand() % 359;
	int angle2 = rand() % 359;
	UVector u;
	u.uvec.x = cos(float(angle));
	u.uvec.y = sin(float(angle));
	//lisää z
	return u;
}

UVector PerlinNoise::GenRandomUnitVector3D() {
	int angle = rand() % 359;
	int angle2 = rand() % 359;
	int angle3 = rand() % 359;
	UVector u;
	u.uvec.x = cos(float(angle));
	u.uvec.y = sin(float(angle));
	u.uvec.z = 0;
	glm::mat4 mRotate = glm::rotate(glm::mat4(1.0f), glm::radians(float(angle3)), glm::vec3(0, 1, 0));
	glm::vec4 v = glm::vec4(u.uvec, 1) * mRotate;
	u.uvec = glm::vec3(v.x, v.y, v.z);
	//lisää z
	return u;
}

void PerlinNoise::GenPerlinNoise1D(const int width, float* texture) {

}

void PerlinNoise::GenPerlinNoise(float* texture) {
	//generate vector grid
	vectorGrid* vg = new vectorGrid[gridDimensions.x * gridDimensions.y]; 
	int index = 0;
	int gridY = (gridDimensions.x + 1); // 
	int rowfirst = 0;
	for (int i = 0; index < gridDimensions.x * gridDimensions.y;) {
		vg[index].v[0] = &unitv[i];
		vg[index].v[1] = &unitv[i + 1];
		vg[index].v[2] = &unitv[i + gridY];
		vg[index].v[3] = &unitv[i + gridY + 1];
		++index;
		if (index % gridDimensions.x == 0) {
			i = rowfirst + gridY;
			rowfirst = i;
		}
		else {
			++i;
		}
	}
	//get pixel color
	index = -1;
	int gi = -1; //gridindex
	float r_value = 1.0f;
	for (int i = 0; i < height; ++i) {
		if (i % (gridPixelDimensions.y) == 0) { //if moving to higher (y-coord) grid
			gi += gridDimensions.x;
		}
		gi -= gridDimensions.x - 1; //set to first grid on x axis;
		for (int j = 0; j < width; ++j) {
			if (j != 0 && j % (gridPixelDimensions.x) == 0) {
				++gi; //next on x axis
			}
			//distance of the point from the corners
			glm::vec3 d[4];
			float relativeDistanceX = float(j % (gridPixelDimensions.x)) / (gridPixelDimensions.x);
			float relativeDistanceY = float(i % (gridPixelDimensions.y)) / (gridPixelDimensions.y);
			//calculate vector from corner to location in grid
			for (int k = 0; k < 4; ++k) {
				d[k].x = relativeDistanceX;
				d[k].y = relativeDistanceY;
				if (k == 1 || k == 3) {
					d[k].x -= 1;
				}
				if (k > 1) {
					d[k].y -= 1;
				}
			}

			//dot products
			float d0 = vg[gi].v[0]->uvec.x * d[0].x + vg[gi].v[0]->uvec.y * d[0].y;
			float d1 = vg[gi].v[1]->uvec.x * d[1].x + vg[gi].v[1]->uvec.y * d[1].y;
			float d2 = vg[gi].v[2]->uvec.x * d[2].x + vg[gi].v[2]->uvec.y * d[2].y;
			float d3 = vg[gi].v[3]->uvec.x * d[3].x + vg[gi].v[3]->uvec.y * d[3].y;

			float t = relativeDistanceX;
			float jl = t * t * t * (t * (t * 6 - 15) + 10);
			t = relativeDistanceY;
			float jl0 = t * t * t * (t * (t * 6 - 15) + 10);

			float di0 = d0 + jl * (d1 - d0);
			float di1 = d2 + jl * (d3 - d2);
			r_value = di0 + jl0 * (di1 - di0);
			r_value = (r_value + 1) / 2;
			texture[++index] = r_value;
			texture[++index] = r_value;
			texture[++index] = r_value;
			texture[++index] = 1;
		}
	}
}

void PerlinNoise::GenPerlinNoise3D(float* texture)
{
	//build vector grid
	int vgsize = gridDimensions.x * gridDimensions.y * gridDimensions.z;
	vectorGrid3D* vg = new vectorGrid3D[vgsize];
	int index = 0;
	int gridY = (gridDimensions.x + 1); //vertices
	int gridZ = (gridDimensions.x + 1) * (gridDimensions.y + 1);
	int rowfirst = 0;
	int layerfirst = 0;
	int rowindex = 0;
	for (int i = 0; index < vgsize;) {
		vg[index].v[0] = &unitv[i];
		vg[index].v[1] = &unitv[i + 1];
		vg[index].v[2] = &unitv[i + gridY];
		vg[index].v[3] = &unitv[i + gridY + 1];
		vg[index].v[4] = &unitv[i + gridZ];
		vg[index].v[5] = &unitv[i + 1 + gridZ];
		vg[index].v[6] = &unitv[i + gridY + gridZ];
		vg[index].v[7] = &unitv[i + gridY + 1 + gridZ];
		//make it loop
		if (index >= vgsize - gridDimensions.x * gridDimensions.y) {
			vg[index].v[4] = vg[index- (vgsize - gridDimensions.x * gridDimensions.y)].v[0];
			vg[index].v[5] = vg[index- (vgsize - gridDimensions.x * gridDimensions.y)].v[1];
			vg[index].v[6] = vg[index- (vgsize - gridDimensions.x * gridDimensions.y)].v[2];
			vg[index].v[7] = vg[index- (vgsize - gridDimensions.x * gridDimensions.y)].v[3];
		}

		++index;
		if (index % gridDimensions.x == 0) { // next row
			++rowindex;
			if (rowindex % gridDimensions.y == 0) { //next layer
				i = layerfirst + gridZ;
				layerfirst = i;
				rowfirst = i;
			}
			else {
				i = rowfirst + gridY;
				rowfirst = i;
			}
		}
		else {
			++i;
		}
	}
	//get pixel color
	index = -1;
	int gi = 0; //gridindex
	float r_value = 1.0f;
	for (int h = 0; h < depth; ++h) {
		if (h % (gridPixelDimensions.z) == 0 && h != 0) { //if moving to deeper (z-coord) grid
			++gi;
		}
		else if(h != 0) {
			gi -= gridDimensions.x * gridDimensions.y - 1; //set to first grid on x and y axis;
		}
		for (int i = 0; i < height; ++i) {
			if (i % (gridPixelDimensions.y) == 0 && i != 0) { //if moving to higher (y-coord) grid
				++gi;
			}
			else if(i != 0){
				gi -= gridDimensions.x - 1; //set to first grid on x axis;
			}
			for (int j = 0; j < width; ++j) {
				if (j != 0 && j % (gridPixelDimensions.x) == 0) {
					++gi; //next on x axis
				}

				//distance of the point from the corners
				glm::vec3 d[8];
				float relativeDistanceX = float(j % (gridPixelDimensions.x)) / (gridPixelDimensions.x);
				float relativeDistanceY = float(i % (gridPixelDimensions.y)) / (gridPixelDimensions.y);
				float relativeDistanceZ = float(h % (gridPixelDimensions.z)) / (gridPixelDimensions.z);
				int k = 0;
				for (int z = 0; z < 2; ++z) {
					for (int y = 0; y < 2; ++y) {
						for (int x = 0; x < 2; ++x) {
							d[k].x = relativeDistanceX;
							d[k].y = relativeDistanceY;
							d[k].z = relativeDistanceZ;
							if (x == 1) {
								d[k].x -= 1;
							}
							if (y == 1) {
								d[k].y -= 1;
							}
							if (z == 1) {
								d[k].z -= 1;
							}
							++k;
						}
					}
				}

				float d0 = vg[gi].v[0]->uvec.x * d[0].x + vg[gi].v[0]->uvec.y * d[0].y + vg[gi].v[0]->uvec.z * d[0].z;
				float d1 = vg[gi].v[1]->uvec.x * d[1].x + vg[gi].v[1]->uvec.y * d[1].y + vg[gi].v[1]->uvec.z * d[1].z;
				float d2 = vg[gi].v[2]->uvec.x * d[2].x + vg[gi].v[2]->uvec.y * d[2].y + vg[gi].v[2]->uvec.z * d[2].z;
				float d3 = vg[gi].v[3]->uvec.x * d[3].x + vg[gi].v[3]->uvec.y * d[3].y + vg[gi].v[3]->uvec.z * d[3].z;
				float d4 = vg[gi].v[4]->uvec.x * d[4].x + vg[gi].v[4]->uvec.y * d[4].y + vg[gi].v[4]->uvec.z * d[4].z;
				float d5 = vg[gi].v[5]->uvec.x * d[5].x + vg[gi].v[5]->uvec.y * d[5].y + vg[gi].v[5]->uvec.z * d[5].z;
				float d6 = vg[gi].v[6]->uvec.x * d[6].x + vg[gi].v[6]->uvec.y * d[6].y + vg[gi].v[6]->uvec.z * d[6].z;
				float d7 = vg[gi].v[7]->uvec.x * d[7].x + vg[gi].v[7]->uvec.y * d[7].y + vg[gi].v[7]->uvec.z * d[7].z;

				float t = relativeDistanceX;
				float jl = t * t * t * (t * (t * 6 - 15) + 10);
				t = relativeDistanceY;
				float jl0 = t * t * t * (t * (t * 6 - 15) + 10);
				t = relativeDistanceZ;
				float jl1 = t * t * t * (t * (t * 6 - 15) + 10);

				float di0 = d0 + jl * (d1 - d0);
				float di1 = d2 + jl * (d3 - d2);
				float di2 = d4 + jl * (d5 - d4);
				float di3 = d6 + jl * (d7 - d6);
				float r0 = di0 + jl0 * (di1 - di0);
				float r1 = di2 + jl0 * (di3 - di2);
				r_value = r0 + jl1 * (r1 - r0);
				r_value = (r_value + 1) / 2;
				//float decol = gi * (1.0f / vgsize);
				texture[++index] = r_value;
				texture[++index] = r_value;
				texture[++index] = r_value;
				texture[++index] = 1;
			}
		}
	}
	delete[](vg);
}

//WorleyNoise

WorleyNoise::WorleyNoise(int wi, int he, int g, float r) {
	width = wi;
	height = he;
	grids = g + 2;
	gridsize = wi / g + 1;
	range = r;
}

void WorleyNoise::GenWorleyNoise(float* texture, int octave) {
	UVector** grid = new UVector * [grids];
	for (int i = 0; i < grids; ++i) {
		grid[i] = new UVector[grids];
		for (int j = 0; j < grids; ++j) {
			UVector u;
			u.uvec.x = float(rand() % 100) / 100.0f;
			u.uvec.y = float(rand() % 100) / 100.0f;
			grid[i][j] = u;
		}
	}

	int index = -1;
	int k = 1, l = 1; //tells us where we are in the grid
	for (int i = 0; i < height; ++i) {
		k = 1;
		if (i % gridsize == 0 && i != 0) {
			++l;
		}
		for (int j = 0; j < width; ++j) {
			if (j % gridsize == 0 && j != 0) {
				++k;
			}
			float posx = float(j - float((k - 1) * gridsize)) / (float)gridsize;
			float posy = float(i - float((l - 1) * gridsize)) / (float)gridsize;
			float mdist = 100.0f;
			float tx, ty;
			int minx = -1, miny = -1, maxx = 1, maxy = 1;
			/*if (posx > 0.75f) {
				minx = 0;
			}
			else if (posx < 0.25f) {
				maxx = 0;
			}
			if (posy > 0.75f) {
				miny = 0;
			}
			else if(posy < 0.25f){
				maxy = 0;
			}*/
			for (int o = minx; o <= maxx; ++o) {
				for (int p = miny; p <= maxy; ++p) {
					if (o == 0) {
						tx = fabsf(grid[k][l + p].uvec.x - posx);
					}
					else {
						tx = o + grid[k + o][l + p].uvec.x - posx;
					}
					if (p == 0) {
						ty = fabsf(grid[k + o][l].uvec.y - posy);
					}
					else {
						ty = p + grid[k + o][l + p].uvec.y - posy;
					}
					//float dist =sqrtf(powf(fabsf(o+grid[k+o][l+p].x-posx),2)+ powf(fabsf(p + grid[k + o][l + p].y - posy), 2));
					float dist = powf(tx, 2) + powf(ty, 2);
					if (dist < mdist) {
						mdist = dist;
					}
				}
			}
			mdist = sqrtf(mdist);
			if (mdist > 1) {
				mdist = 1;
			}
			float r_value = mdist;
			texture[++index] = r_value;
			texture[++index] = r_value;
			texture[++index] = r_value;
			texture[++index] = 1;
		}
	}
}


//FractalLine

FractalLine::FractalLine() {}

FractalLine::FractalLine(int tx, int ty, int tx1, int ty1) {
	c0 = glm::vec3(tx, ty, 0);
	c1 = glm::vec3(tx1, ty1, 0);
}

FractalLine::FractalLine(glm::vec3 tc0, glm::vec3 tc1) {
	c0 = tc0;
	c1 = tc1;
}

void FractalLine::Set(glm::vec3 tc0, glm::vec3 tc1) {
	c0 = tc0;
	c1 = tc1;
}

void FractalLine::AddLine(int x, int y) {
	if (fl.empty()) {
		fl.push_back(new FractalLine(c0, c1));
	}
	fl.push_back(new FractalLine(c1, glm::vec3(x, y, 0)));
	c1 = glm::vec3(x, y, 0);
}

void FractalLine::AddLines(glm::vec3* coord, int amount) {
	int m = fl.size();
	if (m == 0)
		fl.push_back(new FractalLine(c0, c1));
	fl.push_back(new FractalLine(c1, coord[0]));
	for (int i = 1; i < amount; ++i) {
		fl.push_back(new FractalLine(coord[i - 1], coord[i]));
	}
	c1 = coord[amount - 1 + m];
}

void FractalLine::CloseLines() {
	fl.push_back(new FractalLine(fl.back()->c1, c0));
	c1 = c0;
}

void FractalLine::PrintPoints() {
	if (fl.empty())
		return;
	for each (FractalLine * fra in fl)
	{
		std::cout << fra->c0.x << " " << fra->c0.y << "\n";
	}
}

void FractalLine::IterFractal() {
	if (!fl.empty()) {
		for each (FractalLine * fra in fl)
		{
			fra->IterFractal();
		}
	}
	else {
		glm::vec3 nl0[3][6] = {
			{glm::vec3(0,0,0), glm::vec3(0.5f,-0.3f,0), glm::vec3(0.6f,-0.1f,0), glm::vec3(0.75f,-0.25f,0), glm::vec3(0.75f,0,0), glm::vec3(1,0,0)},
			{glm::vec3(0,0,0), glm::vec3(0.4f,0,0), glm::vec3(0.4f,-0.25f,0), glm::vec3(0.75f,-0.25f,0), glm::vec3(0.75f,0,0), glm::vec3(1,0,0) },
			{glm::vec3(0,0,0), glm::vec3(0.25f,0.0f,0), glm::vec3(0.3f,-0.1f,0), glm::vec3(0.5f,-0.25f,0), glm::vec3(0.75f,0,0), glm::vec3(1,0,0) }
		};
		int r = rand() % 3;
		int m = 1;
		if (rand() % 2 == 0) {
			m = -1;
		}
		glm::vec3 nl1[6];
		//x0 = 0, x1 = 0.5f, x2 = 0.5f , x3= 0.75f, x4 = 0.75f, x5 = 1, y0 = 0, y1 = 0, y2 = 0.25f, y3 = 0.25f, y4 = 0, y5 = 0;
		float lineLength = sqrtf(powf(c1.x - c0.x, 2) + powf(c1.y - c0.y, 2));
		float angle = acosf(float(c1.x - c0.x) / lineLength);
		//abs(c1.x - c0.x) > abs(c1.y - c0.y) , angle*180/3.1415 > 160
		if (c1.y - c0.y < 0 && c1.x - c0.x > 0) {
			angle = 3.14159 * 1.5 + fabsf(asinf(float(c1.x - c0.x) / lineLength));
			//std::cout << "XY:n kaut\n";
		}
		else if (c1.y - c0.y < 0) {
			angle = 3.14159 + fabsf(asinf(float(c1.y - c0.y) / lineLength));
			//std::cout << "Y:n kaut\n";
		}
		//std::cout <<angle <<" "<<angle * 180 / 3.1415 <<"\n";
		if (lineLength == 0) {
			angle = 0;
		}
		for (int i = 0; i < 6; ++i) {
			nl1[i].x = (nl0[r][i].x * cosf(angle) - m * nl0[r][i].y * sinf(angle)) * lineLength + c0.x;
			nl1[i].y = (m * nl0[r][i].y * cosf(angle) + nl0[r][i].x * sinf(angle)) * lineLength + c0.y;
		}
		//if (c1.y != round(nl1[5].y)) {
		//	//std::cout << "problem\n";
		//}
		//std::cout << c1.x << " " << c1.y << " vrt " << nl1[5].x << " " << nl1[5].y << "\n\n";
		fl.push_back(new FractalLine(c0, glm::vec3(nl1[1].x, nl1[1].y, 0)));
		fl.push_back(new FractalLine(glm::vec3(nl1[1].x, nl1[1].y, 0), glm::vec3(nl1[2].x, nl1[2].y, 0)));
		fl.push_back(new FractalLine(glm::vec3(nl1[2].x, nl1[2].y, 0), glm::vec3(nl1[3].x, nl1[3].y, 0)));
		fl.push_back(new FractalLine(glm::vec3(nl1[3].x, nl1[3].y, 0), glm::vec3(nl1[4].x, nl1[4].y, 0)));
		fl.push_back(new FractalLine(glm::vec3(nl1[4].x, nl1[4].y, 0), c1));
	}
}

//float2 nl0[6] = {float2(0,0), float2(0.5f,0), float2(0.5f,0.25f), float2(0.75f,0.25f), float2(0.75f,0), float2(1,0)};