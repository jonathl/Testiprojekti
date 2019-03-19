#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <math.h>




struct UVector
{
	float x;
	float y;
	int posx;
	int posy;
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

	PerlinNoise(int wi, int he, int g) {
		width = wi;
		height = he;
		grids = pow(2,g);
		gridsize = wi / grids +1;
		int vectorsize = (grids + 1)*(grids + 1);
		int t = 0; 
		unitv = new UVector[vectorsize];
		for (int i = 0; i < vectorsize; ++i) {  //generate unit vectors
			unitv[i] = m_genRandomUnitVector();
			unitv[i].posx = width / grids * t;
			unitv[i].posy = height / grids * (i/(grids+1));
			++t;
			if (t > grids)
				t = 0;
		}
	}

	UVector m_genRandomUnitVector() {
		int angle = rand() % 359;
		UVector u;
		u.x = cos(float(angle));
		u.y = sin(float(angle));
		return u;
	}

	void m_octavePerlin(float *texture) {
		float* pic = new float[width*height];
		float* pictemp = new float[width*height];
		m_genPerlinNoise(pic, 1);
		int octave = 2;
		int index;
		while (octave <= grids) {
			index = -1;
			m_genPerlinNoise(pictemp, octave);
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

	void m_genPerlinNoise1D(const int width, float *texture) {

	}

	void m_genPerlinNoise(float *texture, int octave) { 
		octave = pow(2, octave);
		if (octave > grids) {
			std::cout << "we have a problem\n";
			return;
		}
		if ((gridsize * grids) - width > gridsize) {
			std::cout << "picture too small for that octave\n";
			return;
		}
		vectorGrid* vg = new vectorGrid[grids*grids]; //generate vector grid
		int index = 0;
		int go = grids / octave;
		int go0 = (grids + 1)*(go);
		int rowfirst = 0;
		for (int i = 0; i < (grids+1)*(grids);) { 
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
			if (i % (gridsize * (grids/octave)) == 0) {
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
					dx[k] = float(j % (gridsize*go))/ (gridsize*go);
					dy[k] = float(i % (gridsize*go)) / (gridsize*go);
					if (k == 1 || k == 3) {
						dx[k] = dx[k] - 1;
					}
					if (k > 1) {
						dy[k] = dy[k] - 1;
					}
				}
				float d0 = vg[gi].v[0]->x * dx[0] + vg[gi].v[0]->y * dy[0];
				float d1 = vg[gi].v[1]->x * dx[1] + vg[gi].v[1]->y * dy[1];
				float d2 = vg[gi].v[2]->x * dx[2] + vg[gi].v[2]->y * dy[2];
				float d3 = vg[gi].v[3]->x * dx[3] + vg[gi].v[3]->y * dy[3];

				float t = float(j%(gridsize * (grids / octave))) / (gridsize * (grids / octave));
				float jl = t * t * t * (t * (t * 6 - 15) + 10);
				t = float(i%(gridsize* (grids / octave))) / (gridsize* (grids / octave));
				float jl0 = t * t * t * (t * (t * 6 - 15) + 10);

				float di0 = d0 + jl * (d1 - d0);
				float di1 = d2 + jl * (d3 - d2);
				r_value = di0 + jl0 * (di1 - di0);
				r_value = (r_value + 1) / 2;
				texture[++index] = r_value;
				texture[++index] = r_value; 
				texture[++index] = r_value;
			}
		}
	}

};

class WorleyNoise {
private:
	int width;
	int height;
	int gridsize; //size in pixels of grid
	int grids; //grids x grids = grid
	float range;

public:
	WorleyNoise(int wi, int he, int g, float r) {
		width = wi;
		height = he;
		grids = g + 2;
		gridsize = wi / g + 1;
		range = r;
	}
	
	void m_genWorleyNoise(float *texture, int octave) {
		UVector** grid = new UVector*[grids];
		for (int i = 0; i < grids; ++i) {
			grid[i] = new UVector[grids];
			for (int j = 0; j < grids; ++j) {
				UVector u;
				u.x = float(rand() % 100) / 100.0f;
				u.y = float(rand() % 100) / 100.0f;
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
				float posx = float(j - float((k - 1)*gridsize)) / (float)gridsize;
				float posy = float(i - float((l - 1)*gridsize)) / (float)gridsize;
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
							tx = fabsf(grid[k][l + p].x - posx);
						}
						else {
							tx = o + grid[k + o][l + p].x - posx;
						}
						if (p == 0) {
							ty = fabsf(grid[k + o][l].y - posy);
						}
						else {
							ty = p + grid[k + o][l + p].y - posy;
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
			}
		}
	}
};



//void m_genRandomNoise(int width, int height, float *texture) {
//	int index = -1;
//	float r_value;
//	for (int i = 0; i < height; ++i) {
//		for (int j = 0; j < width; ++j) {
//			r_value = float(rand() % 100) / 100;
//			texture[++index] = r_value;
//			texture[++index] = r_value;
//			texture[++index] = r_value;
//		}
//	}
//}