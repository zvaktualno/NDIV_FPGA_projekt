#include <stdio.h>
#include <stdint.h>
#include "xparameters.h"
#include "xil_io.h"
#include "sleep.h"
#include <math.h>

#define COLOR_BLUE 0x3
#define COLOR_RED 0xE0
#define COLOR_GREEN 0x1D
#define COLOR_BLACK 0x00

#define SCREEN_WIDTH 250
#define SCREEN_HEIGHT SCREEN_WIDTH
typedef struct _point3D{
	int16_t x;
	int16_t y;
	int16_t z;
} point3D;


point3D createPoint3D(int8_t x0, int8_t y0, int8_t z0){
	point3D tmp = {.x=x0, .y=y0, .z=z0};
	return tmp;
}


void setLineColor(uint8_t color){
	//RRRGGGBB
	Xil_Out8(XPAR_AXI_SET4_0_S00_AXI_BASEADDR+0, color); // pixel
	usleep(10);
}

void setBackgroundColor(uint8_t color){
	Xil_Out8(XPAR_AXI_SET4_0_S00_AXI_BASEADDR+4, color); // background
	usleep(10);
}


void drawLine3D(point3D p1, point3D p2){
	const int16_t offset = SCREEN_WIDTH/2;
	p1.x += offset;
	p2.x += offset;
	p1.y += offset;
	p2.y += offset;
	uint16_t first_point = p1.x << 8 | p1.y;
	uint16_t second_point = p2.x << 8 | p2.y;
	Xil_Out16(XPAR_AXI_SET4_0_S00_AXI_BASEADDR+8, first_point); // pixel
	Xil_Out16(XPAR_AXI_SET4_0_S00_AXI_BASEADDR+12, second_point);
	usleep(10);
}

point3D rotatePoint3D(point3D p, float xangle, float yangle, float zangle){
	point3D point=p;

	xangle = 6.283 * xangle/360.0;
	yangle = 6.283 * yangle/360.0;
	zangle = 6.283 * zangle/360.0;

	//xangle
	point.y = p.y*cos(xangle)-p.z*sin(xangle);
	point.z = p.y*sin(xangle)+p.z*cos(xangle);
	p=point;

	//yangle
	point.x = p.x*cos(yangle)+p.z*sin(yangle);
	point.z = p.x*cos(yangle)-p.z*sin(yangle);
	p=point;

	//zangle
	point.x = p.x*cos(zangle)-p.y*sin(zangle);
	point.y = p.x*sin(zangle)+p.y*cos(zangle);
	p=point;
	return p;
}

typedef struct _cube
{
	point3D p0;
	point3D p1;
	point3D p2;
	point3D p3;
	point3D p4;
	point3D p5;
	point3D p6;
	point3D p7;
}cube;

void drawCube(cube cub){
	drawLine3D(cub.p0, cub.p2);
	drawLine3D(cub.p0, cub.p1);
	drawLine3D(cub.p0, cub.p4);
	drawLine3D(cub.p6, cub.p4);
	drawLine3D(cub.p6, cub.p2);
	drawLine3D(cub.p6, cub.p7);
	drawLine3D(cub.p2, cub.p3);
	drawLine3D(cub.p3, cub.p1);
	drawLine3D(cub.p3, cub.p7);
	drawLine3D(cub.p5, cub.p1);
	drawLine3D(cub.p5, cub.p4);
	drawLine3D(cub.p5, cub.p7);
}

cube rotateCube(cube _cube, float xangle, float yangle, float zangle){
	cube tmp_cube=_cube;
	tmp_cube.p0 = rotatePoint3D(_cube.p0, xangle,yangle,zangle);
	tmp_cube.p1 = rotatePoint3D(_cube.p1, xangle,yangle,zangle);
	tmp_cube.p2 = rotatePoint3D(_cube.p2, xangle,yangle,zangle);
	tmp_cube.p3 = rotatePoint3D(_cube.p3, xangle,yangle,zangle);
	tmp_cube.p4 = rotatePoint3D(_cube.p4, xangle,yangle,zangle);
	tmp_cube.p5 = rotatePoint3D(_cube.p5, xangle,yangle,zangle);
	tmp_cube.p6 = rotatePoint3D(_cube.p6, xangle,yangle,zangle);
	tmp_cube.p7 = rotatePoint3D(_cube.p7, xangle,yangle,zangle);
	return tmp_cube;
}

cube createCubeSize(uint16_t size){
	cube cub;
	cub.p0 = createPoint3D(-size,-size,-size);
	cub.p1 = createPoint3D(-size,-size,size);
	cub.p2 = createPoint3D(-size,size,-size);
	cub.p3 = createPoint3D(-size,size,size);
	cub.p4 = createPoint3D(size,-size,-size);
	cub.p5 = createPoint3D(size,-size,size);
	cub.p6 = createPoint3D(size,size,-size);
	cub.p7 = createPoint3D(size,size,size);
	return cub;
}

int main()
{
	setLineColor(COLOR_RED);
	setBackgroundColor(COLOR_BLACK);


	float size_angle = 0;
	uint8_t size = 50;
	uint8_t deltaSize = 20;
	uint8_t rotation_speed = 10;
	float xangle=0,yangle=0,zangle=0;
	while(1){
		cube cub = createCubeSize(size+deltaSize*sin(size_angle));
		size_angle += 3.14/180.0*rotation_speed/5;
		cube tmpCube =rotateCube(cub,xangle,yangle,zangle);
		xangle += 1;
		yangle += 1;
		zangle += 0;
		setLineColor(COLOR_RED);
		drawCube(tmpCube);
		usleep(10000);
		setLineColor(COLOR_BLACK);
		drawCube(tmpCube);
	}


    return 0;
}
