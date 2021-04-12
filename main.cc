#include <fstream>
#include <cmath>
#include <cstdint>
#include <vector>
#include <cassert>

#pragma pack(push, 1)
struct TGA_Header{
	uint8_t id_len;			// Длина идентификатора
	uint8_t pal_type;		// Тип палитры
	uint8_t img_type;		// Тип изображения
	uint8_t pal_desc[5];	// Описание палитры
	uint16_t x_pos;			// Положение по оси X
	uint16_t y_pos;			// Положение по оси Y
	uint16_t width;			// Ширина
	uint16_t height;		// Высота
	uint8_t depth;			// Глубина цвета
	uint8_t img_desc;		//Описатель изображения
};
#pragma pack(pop)

constexpr uint16_t IMG_WIDTH = 1440;
constexpr uint16_t IMG_HEIGHT = 900;
constexpr uint32_t COL_BACKGROUND = 0xFF4B0082;
constexpr uint32_t COL_FOREGROUND = 0xFFFF0000;

constexpr double scale_x = 15, scale_y = 15., scale_z = 250.,
				   max_x = 25., min_x = -25., max_y = 25., min_y = -25.,
				   s1 = 0.5, s2 = 0.01, angle = M_PI/8;

double sinc(double x){
	if (x == 0) return 1.;
	return sin(x)/x;
}



double my_evil_function(double x, double y){
	return sinc(hypot(x, y));
}
int main(){
	std::vector<uint32_t> picture(IMG_WIDTH * IMG_HEIGHT);
	for (auto && p : picture) p = COL_BACKGROUND;

	std::vector<int> horizon(IMG_WIDTH);
	for (auto && p : horizon) p = IMG_HEIGHT;

	int sX = 0, sY = 0;
	double z;
	for (double x = max_x; x >= min_x; x -= s1){
		for (double y = max_y; y >= min_y; y -= s2){
			z = my_evil_function(x,y);
			sX = int(IMG_WIDTH / 2 - scale_x * x * cos(angle) + scale_y * y * cos(angle));
			sY = int(IMG_HEIGHT / 2 + scale_x * x * sin(angle) + scale_y * y * sin(angle) - scale_z * z);
			if (sX >= 0 && sX < IMG_WIDTH && sY <= horizon[sX]){
				horizon[sX]=sY;
				if (sY >= 0 && sY * IMG_WIDTH + sX >= 0 && sY * IMG_WIDTH + sX < IMG_WIDTH * IMG_HEIGHT){
					picture[sY * IMG_WIDTH + sX] = COL_FOREGROUND;
				}
			}
		}
	}
	for (auto && p : horizon) p = IMG_HEIGHT;

	for (double x = max_x; x >= min_x; x -= s2){
		for (double y = max_y; y >= min_y; y -= s1){
			z = my_evil_function(x,y);
			sX = int(IMG_WIDTH / 2 - scale_x * x * cos(angle) + scale_y * y * cos(angle));
			sY = int(IMG_HEIGHT / 2 + scale_x * x * sin(angle) + scale_y * y * sin(angle) - scale_z * z);
			if (sX >= 0 && sX < IMG_WIDTH && sY <= horizon[sX]){
				horizon[sX]=sY;
				if (sY >= 0 && sY * IMG_WIDTH + sX >= 0 && sY * IMG_WIDTH + sX < IMG_WIDTH * IMG_HEIGHT){
					picture[sY * IMG_WIDTH + sX] = COL_FOREGROUND;
				}
			}
		}
	}



	TGA_Header hdr {};
	hdr.width = IMG_WIDTH;
	hdr.height = IMG_HEIGHT;
	hdr.depth = 32;
	hdr.img_type = 2;
	hdr.img_desc = 0x28;

	std::fstream tga_file { "output.tga", std::ios::binary };
	tga_file.open("output.tga",std::ios::out|std::ios::binary);
	tga_file.write(reinterpret_cast<char*>(&hdr),sizeof(hdr));
	tga_file.write(reinterpret_cast<char*>(&picture[0]), picture.size() * sizeof(uint32_t));
	tga_file.close();
	return 0;
}

