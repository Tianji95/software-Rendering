#pragma once
#ifndef _DRAWELEMENT_H_
#define _DRAWELEMENT_H_
#include<Windows.h>
#include"Transform.h"
#include"MyMath.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TEXTURE_MODE 4
#define COLOR_MODE 2
#define WINDOWS_CLASS_NAME "DrawElement"
#define PI 3.1415926536

class Scanline {
public:
	Scanline();
	~Scanline();
	int y;
	double x;
	double width;
	point leftPoint, rightPoint;
	point stepPoint;
	vect oriLeftVect, oriRightVect;
	vect oriStepVect;
};
class Trapezoid {
public:
	Trapezoid();
	~Trapezoid();
	double top;
	double bottom;
	point left1, left2;
	point right1, right2;
	vect oriLeft1, oriLeft2;
	vect oriRight1, oriRight2;
};



class DrawElement {
public:

	DrawElement();
	~DrawElement();
	HRESULT initDevice();
	void clearBuffer();
	void destoryDevice();
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void drawTriangle(point* v1, point* v2, point*v3);
	int divideTriangle(point* finalV1, point* finalV2, point* finalV3, Trapezoid* trape1, Trapezoid* trape2, vect* oriV1, vect* oriV2, vect* oriV3);
	void randerTriangle(Trapezoid* trape);
	void initScanLine(Scanline *scline, int yPos, Trapezoid* trape);
	void drawScanLine(Scanline *scline);
	void updateScene();
	void loadTexture(char* filename);


	UINT renderMode;
	Transform transform;//����任
	vect lightDiffuse;//
	vect lightSpecular;
	vect lightAmbient;
	vect lightPos;
	vect cameraPosition;
	vect cameraTarget;
	vect cameraUp;
	HDC hMemDC;//һ�������豸���豸�������
	HBITMAP hBITMAP;//һ��λͼ���
	HDC hdc;//windows�豸�������

	LARGE_INTEGER startTick = {};
	LARGE_INTEGER endTick = {};
	LARGE_INTEGER frequency_ = {};
	FLOAT fps_ = 60.0f;

private:
	float ks = 1;//�߹�ϵ��
	float kd = 1;//�������ϵ��
	float ka = 1;//������ϵ��
	float ke = 0.1f; //�Է����ϵ��
	int shininess = 3;
	int textureWidth;
	int textureHeight;
	int textureChannal;
	HINSTANCE hinstSelf;

	HWND hwnd;//windows  ������

	BITMAPINFO bmpInfo;//��Ҫ����λͼ��Ϣ
	unsigned char *textureBuffer;
	UINT32 *bmpBuffer_;
	double *zBuffer_;
	UINT backGroundColor;

};

#endif // !_DRAWELEMENT_H_
