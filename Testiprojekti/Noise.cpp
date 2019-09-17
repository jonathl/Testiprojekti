#include "Noise.h"

//PerlinNoise

PerlinNoise::PerlinNoise(int wi, int he, int g) {
	width = wi;
	height = he;
	grids = pow(2, g);
	gridsize = wi / grids + 1;
	int vectorsize = (grids + 1) * (grids + 1);
	int t = 0;
	unitv = new UVector[vectorsize];
	for (int i = 0; i < vectorsize; ++i) {  //generate unit vectors
		unitv[i] = GenRandomUnitVector();
		unitv[i].posx = width / grids * t;
		unitv[i].posy = height / grids * (i / (grids + 1));
		++t;
		if (t > grids)
			t = 0;
	}
}

UVector PerlinNoise::GenRandomUnitVector() {
	int angle = rand() % 359;
	UVector u;
	u.uvec.x = cos(float(angle));
	u.uvec.y = sin(float(angle));
	//lis‰‰ z
	return u;
}

void PerlinNoise::OctavePerlin(float* texture) {
	float* pic = new float[width * height];
	float* pictemp = new float[width * height];
	GenPerlinNoise(pic, 1);
	int octave = 2;
	int index;
	while (octave <= grids) {
		index = -1;
		GenPerlinNoise(pictemp, octave);
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				pic[++index] = pic[index] + 0.5f * (pictemp[index] - pic[index]);
			}
		}
		octave *= 2;
	}
	index = -1;
	int index1 = -1;
	float r_value;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			r_value = pic[++index1];
			texture[++index] = r_value;
			texture[++index] = r_value;
			texture[++index] = r_value;
		}
	}
}

void PerlinNoise::GenPerlinNoise1D(const int width, float* texture) {

}

void PerlinNoise::GenPerlinNoise(float* texture, int octave) {
	octave = pow(2, octave);
	if (octave > grids) {
		std::cout << "we have a problem\n";
		return;
	}
	if ((gridsize * grids) - width > gridsize) {
		std::cout << "picture too small for that octave\n";
		return;
	}
	vectorGrid* vg = new vectorGrid[grids * grids]; //generate vector grid
	int index = 0;
	int go = grids / octave;
	int go0 = (grids + 1) * (go);
	int rowfirst = 0;
	for (int i = 0; i < (grids + 1) * (grids);) {
		vg[index].v[0] = &unitv[i];
		vg[index].v[1] = &unitv[i + go];
		vg[index].v[2] = &unitv[i + go0];
		vg[index].v[3] = &unitv[i + go0 + go];
		++index;
		if (index % octave == 0) {
			i = rowfirst + go0;
			rowfirst = i;
		}
		else {
			i += go;
		}
	}
	index = -1;
	int gi = -1; //gridindex
	float r_value = 1.0f;
	for (int i = 0; i < height; ++i) {
		if (i % (gridsize * (grids / octave)) == 0) {
			gi += octave;
		}
		gi -= octave - 1;
		for (int j = 0; j < width; ++j) {
			if (j != 0 && j % (gridsize * (grids / octave)) == 0) {
				++gi;
			}
			float dx[4];
			float dy[4];
			for (int k = 0; k < 4; ++k) {
				dx[k] = float(j % (gridsize * go)) / (gridsize * go);
				dy[k] = float(i % (gridsize * go)) / (gridsize * go);
				if (k == 1 || k == 3) {
					dx[k] = dx[k] - 1;
				}
				if (k > 1) {
					dy[k] = dy[k] - 1;
				}
			}
			float d0 = vg[gi].v[0]->uvec.x * dx[0] + vg[gi].v[0]->uvec.y * dy[0];
			float d1 = vg[gi].v[1]->uvec.x * dx[1] + vg[gi].v[1]->uvec.y * dy[1];
			float d2 = vg[gi].v[2]->uvec.x * dx[2] + vg[gi].v[2]->uvec.y * dy[2];
			float d3 = vg[gi].v[3]->uvec.x * dx[3] + vg[gi].v[3]->uvec.y * dy[3];

			float t = float(j % (gridsize * (grids / octave))) / (gridsize * (grids / octave));
			float jl = t * t * t * (t * (t * 6 - 15) + 10);
			t = float(i % (gridsize * (grids / octave))) / (gridsize * (grids / octave));
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
			{glm::vec3(0,0,0), glm::vec3(0.5f,0,0), glm::vec3(0.5f,-0.25f,0), glm::vec3(0.75f,-0.25f,0), glm::vec3(0.75f,0,0), glm::vec3(1,0,0) },
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