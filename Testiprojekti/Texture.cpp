#include "Texture.h"

Texture::Texture(int wi, int he) {
	width = wi;
	height = he;
	texelCount = wi * he;
	pic = new float[texelCount*4];
}

int Texture::GetTextureCoordinate(int x, int y) {
	int textcoord = width * (height - y) - x;
	//std::cout << "texcoord = " << textcoord << "\n";
	return textcoord*4;
}

void Texture::DrawToTexture(int x, int y) { //korjaa reunan yli piirtäminen ja textuurin ko-on vaikuttaminen
	float color[] = { 1.0f, 0.0f, 0.0f };
	int index = GetTextureCoordinate(x, y);
	pic[index] = color[0];
	pic[++index] = color[1];
	pic[++index] = color[2];
	pic[++index] = 1.0f;
}

//kesken
//void Texture::colorAdjacent(float* color, float* fillcolor, int texel, float* texture, int width, int height) { //poista recursio, stackki täyttyy
//	if ((color[0] != fillcolor[0]) && (color[1] != fillcolor[1]) && (color[2] != fillcolor[2])) {
//		texture[texel] = fillcolor[0];
//		texture[texel + 1] = fillcolor[1];
//		texture[texel + 2] = fillcolor[2];
//		if (texel > width * 3) { //up
//			colorAdjacent(color, fillcolor, texel - width * 3, texture, width, height);
//		}
//		if (texel % (width * 3) != 0) { //left
//			colorAdjacent(color, fillcolor, texel - 3, texture, width, height);
//		}
//		if (texel % (width * 3) != (width * 3) - 1) { //right
//			colorAdjacent(color, fillcolor, texel + 3, texture, width, height);
//		}
//		if (texel < width * 3 * (height - 1)) { //down
//			colorAdjacent(color, fillcolor, texel + width * 3, texture, width, height);
//		}
//	}
//}
//
//void Texture::bucketToolTexture(int cursorX, int cursorY) {
//	float fillcolor[] = { 1.0f, 1.0f, 1.0f };
//	int index = getTextureCoordinate(cursorX, cursorY) * 3;
//	float color[] = { pic[index], pic[index + 1], pic[index + 2] };// onnistuu ylittämään max arvon??
//	colorAdjacent(color, fillcolor, index, pic, width, height);
//}
//
//void Texture::bucketToolTexture2(int cursorX, int cursorY) {
//	float fillcolor[] = { 1.0f, 1.0f, 1.0f };
//	int o = getTextureCoordinate(cursorX, cursorY) * 3;
//	int index = o;
//	int oy = o;
//	float color[] = { pic[index], pic[index + 1], pic[index + 2] };
//
//	while ((pic[index] != fillcolor[0]) && (pic[index + 1] != fillcolor[1]) && (pic[index + 2] != fillcolor[2])) { //vertaa coloriin
//		while ((pic[index] != fillcolor[0]) && (pic[index + 1] != fillcolor[1]) && (pic[index + 2] != fillcolor[2])) {
//			pic[index] = fillcolor[0];
//			pic[index + 1] = fillcolor[1];
//			pic[index + 2] = fillcolor[2];
//			index += 3;
//		}
//		oy += width * 3;
//		index = oy;
//	}
//}
//


void Texture::GenOneColorTex(float r, float g, float b) {
	int index = -1;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			pic[++index] = r;
			pic[++index] = g;
			pic[++index] = b;
			pic[++index] = 1.0f;
		}
	}
}

void Texture::GenOneColorTex(float r, float g, float b, float a) {
	int index = -1;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			pic[++index] = r;
			pic[++index] = g;
			pic[++index] = b;
			pic[++index] = a;
		}
	}
}

void Texture::GenCheckerboardTex() {
	int state = 1;
	int others = 1;
	int index = -1;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (state == others) {
				pic[++index] = 1.0f;
				pic[++index] = 1.0f;
				pic[++index] = 1.0f;
				pic[++index] = 1.0f;
				
			}
			else {
				pic[++index] = 0.0f;
				pic[++index] = 0.0f;
				pic[++index] = 0.0f;
				pic[++index] = 1.0;
			}
			state *= -1;
		}
		others *= -1;
	}
}

void Texture::DrawGridOnTex(int gridsize) {
	int index = 0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (j % gridsize == 0 || i % gridsize == 0) {
				pic[index] = 1.0f;
				pic[index + 1] = 0.2f;
				pic[index + 2] = 0.7f;
				pic[index + 3] = 1.0f;
			}
			index += 4;
		}
	}
}

void Texture::DrawGridOnTex(int gridsize, float r, float g, float b) {
	int index = 0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (j % gridsize == 0 || i % gridsize == 0) {
				pic[index] = r;
				pic[index + 1] = g;
				pic[index + 2] = b;
				pic[index + 3] = 1.0f;
			}
			index += 4;
		}
	}
}

void Texture::DrawLine(int x0, int y0, int x1, int y1) {

	int index2 = GetTextureCoordinate(x1, y1);
	pic[index2] = 1.0f;
	pic[index2 + 1] = 0.2f;
	pic[index2 + 2] = 0.4f;
	pic[index2 + 3] = 1.0f;
	index2 = GetTextureCoordinate(x0, y0);
	pic[index2] = 1.0f;
	pic[index2 + 1] = 0.2f;
	pic[index2 + 2] = 0.4f;
	pic[index2 + 3] = 1.0f;


	int y2 = y1 - y0;
	int x2 = x1 - x0;
	float a = (float)y2 / (float)x2; //y = ax + b
	float b = y0 - a * x0;
	int m = 1;
	int i = 0;
	float x = x0, y = y0;
	if (x2 == 0) { //x0 = x1
		if (y2 < 0) {
			m = -1;
		}
		while ((round(x) != x1 || round(y) != y1))
		{
			if (i != 0) {
				y += 1 * m;
			}
			int index = GetTextureCoordinate(x, y);
			pic[index] = 1.0f;
			pic[index + 1] = 0.2f;
			pic[index + 2] = 0.4f;
			pic[index + 3] = 1.0f;
			++i;
		}
	}

	else if (y2 == 0) { //y0 = y1
		if (x2 < 0) {
			m = -1;
		}
		while ((round(x) != x1 || round(y) != y1))
		{
			if (i != 0) {
				x += 1 * m;
			}
			int index = GetTextureCoordinate(x, y);
			pic[index] = 1.0f;
			pic[index + 1] = 0.2f;
			pic[index + 2] = 0.4f;
			pic[index + 3] = 1.0f;
			++i;
		}
	}

	else if (a <= 1 && a >= -1) {
		if (x2 < 0) {
			m = -1;
		}
		while ((round(x) != x1 || round(y) != y1))
		{
			x += 1 * m;
			y = roundf(a * x + b);
			int index = GetTextureCoordinate(round(x), y);
			pic[index] = 1.0f;
			pic[index + 1] = 0.2f;
			pic[index + 2] = 0.4f;
			pic[index + 3] = 1.0f;
			++i;
		}
	}

	else {
		if (y2 < 0) {
			m = -1;
		}
		while ((round(x) != x1 || round(y) != y1))
		{
			y += 1 * m;
			x = roundf((y - b) / a);
			int index = GetTextureCoordinate(round(x), y);
			pic[index] = 1.0f;
			pic[index + 1] = 0.2f;
			pic[index + 2] = 0.4f;
			pic[index + 3] = 1.0f;
			++i;
		}
	}
}

void Texture::DrawLine(int2 c0, int2 c1) {
	DrawLine(c0.x, c0.y, c1.x, c1.y);
}

void Texture::DrawFractal(FractalLine f) {
	if (f.fl.empty()) {
		DrawLine(f.c0, f.c1);
	}
	else {
		for each (FractalLine * fra in f.fl) {
			DrawFractal(*fra);
		}
	}
}

//vaihda käyttää texturee
void Texture::CombinePictures(Texture texture, Texture* rtex, float str) {
	for (int i = 0; i < texelCount * 4; ++i)
	{
		rtex->pic[i] = pic[i] + str * (texture.pic[i] - pic[i]);
	}
	/*int index = -1;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			texture[++index] = pic1[++index] + str * (pic2[++index] - pic1[++index]);
			texture[++index] = pic1[++index] + str * (pic2[++index] - pic1[++index]);
			texture[++index] = pic1[++index] + str * (pic2[++index] - pic1[++index]);
		}
	}*/
}

float Texture::ChangeScale(float oldmin, float oldmax, float newmin, float newmax, float value) {
	float p = (value - oldmin) / (oldmax - oldmin);
	return p * (newmax - newmin) + newmin;
}

void Texture::AddContrast(float str, float displacement) {
	if (str == 0) {
		return;
	}
	float min = atanf(2 * 3.141 * str * (-0.5 + displacement)), max = atanf(2 * 3.141 * str * (0.5 + displacement));
	float value;
	for (int i = 0; i < texelCount * 4; ++i)
	{
		value = atanf(2 * 3.141 * str * (pic[i] - 0.5 + displacement));
		pic[i] = ChangeScale(min, max, 0, 1, value);
	}
}

void Texture::GenRandomNoise() {
	int index = -1;
	float r_value;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			r_value = float(rand() % 100) / 100;
			pic[++index] = r_value;
			pic[++index] = r_value;
			pic[++index] = r_value;
			pic[++index] = 1.0f;
		}
	}
}

void Texture::GenRandomNoiseColor() {
	int index = -1;
	float r_value;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			r_value = float(rand() % 100) / 100;
			pic[++index] = r_value;
			r_value = float(rand() % 100) / 100;
			pic[++index] = r_value;
			r_value = float(rand() % 100) / 100;
			pic[++index] = r_value;
			pic[++index] = 1.0f;
		}
	}
}


//void Texture::write_row_callback(png_structp png_ptr, png_uint_32 row, int pass) {
//	//std::cout << row <<"\n";
//}

void Texture::SaveAsPNG(char* file_name, float* buffer, char* title) {
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	FILE* fp = fopen(file_name, "wb");
	if (!fp) {
		std::printf("[write_png_file] File %s could not be opened for writing", file_name);
		return;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		printf("Could not allocate write struct\n");
		return;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		printf("Could not allocate info struct\n");
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("Error during png creation\n");
		fclose(fp);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return;
	}

	png_init_io(png_ptr, fp);

	//aseta toimivaks
	//png_set_write_status_fn(png_ptr, write_row_callback);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	if (title != NULL) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "Title";
		title_text.text = title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}
	png_write_info(png_ptr, info_ptr);


	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep)malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			int i = x * 3;
			row[i] = png_byte(int(buffer[y * width * 3 + i] * 255));
			row[i + 1] = png_byte(int(buffer[y * width * 3 + i + 1] * 255));
			row[i + 2] = png_byte(int(buffer[y * width * 3 + i + 2] * 255));
		}
		png_write_row(png_ptr, row);
		//rows[y] = row;
	}
	//png_write_image(png_ptr, rows);
	// End write
	png_write_end(png_ptr, NULL);

	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);
}

