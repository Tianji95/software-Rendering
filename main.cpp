//´úÂë²Î¿¼×Ô£ºhttps://www.zhihu.com/question/24786878
//https://www.zhihu.com/question/33712299

#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include"MyMath.h"
#include"Transform.h"
#include"DrawElement.h"

using namespace std;
CHAR fpsBuffer[256];
LARGE_INTEGER delta;



double vertexData[36][8] = {
{ -1.0,1.0,-1.0,0.0,0.0,0.0,0.0,-1.0 },
{ 1.0,1.0,-1.0,1.0,0.0,0.0,0.0,-1.0 },
{ -1.0,-1.0,-1.0,0.0,1.0,0.0,0.0,-1.0 },
{ -1.0,-1.0,-1.0,0.0,1.0,0.0,0.0,-1.0 },
{ 1.0,1.0,-1.0,1.0,0.0,0.0,0.0,-1.0 },
{ 1.0,-1.0,-1.0,1.0,1.0,0.0,0.0,-1.0 },
{ 1.0,1.0,-1.0,0.0,0.0,1.0,0.0,0.0 },
{ 1.0,1.0,1.0,1.0,0.0,1.0,0.0,0.0 },
{ 1.0,-1.0,-1.0,0.0,1.0,1.0,0.0,0.0 },
{ 1.0,-1.0,-1.0,0.0,1.0,1.0,0.0,0.0 },
{ 1.0,1.0,1.0,1.0,0.0,1.0,0.0,0.0 },
{ 1.0,-1.0,1.0,1.0,1.0,1.0,0.0,0.0 },
{ 1.0,1.0,1.0,0.0,0.0,0.0,0.0,1.0 },
{ -1.0,1.0,1.0,1.0,0.0,0.0,0.0,1.0 },
{ 1.0,-1.0,1.0,0.0,1.0,0.0,0.0,1.0 },
{ 1.0,-1.0,1.0,0.0,1.0,0.0,0.0,1.0 },
{ -1.0,1.0,1.0,1.0,0.0,0.0,0.0,1.0 },
{ -1.0,-1.0,1.0,1.0,1.0,0.0,0.0,1.0 },
{ -1.0,1.0,1.0,0.0,0.0,-1.0,0.0,0.0 },
{ -1.0,1.0,-1.0,1.0,0.0,-1.0,0.0,0.0 },
{ -1.0,-1.0,1.0,0.0,1.0,-1.0,0.0,0.0 },
{ -1.0,-1.0,1.0,0.0,1.0,-1.0,0.0,0.0 },
{ -1.0,1.0,-1.0,1.0,0.0,-1.0,0.0,0.0 },
{ -1.0,-1.0,-1.0,1.0,1.0,-1.0,0.0,0.0 },
{ -1.0,1.0,1.0,0.0,0.0,0.0,1.0,0.0 },
{ 1.0,1.0,1.0,1.0,0.0,0.0,1.0,0.0 },
{ -1.0,1.0,-1.0,0.0,1.0,0.0,1.0,0.0 },
{ -1.0,1.0,-1.0,0.0,1.0,0.0,1.0,0.0 },
{ 1.0,1.0,1.0,1.0,0.0,0.0,1.0,0.0 },
{ 1.0,1.0,-1.0,1.0,1.0,0.0,1.0,0.0 },
{ -1.0,-1.0,-1.0,0.0,0.0,0.0,-1.0,0.0 },
{ 1.0,-1.0,-1.0,1.0,0.0,0.0,-1.0,0.0 },
{ -1.0,-1.0,1.0,0.0,1.0,0.0,-1.0,0.0 },
{ -1.0,-1.0,1.0,0.0,1.0,0.0,-1.0,0.0 },
{ 1.0,-1.0,-1.0,1.0,0.0,0.0,-1.0,0.0 },
{ 1.0,-1.0,1.0,1.0,1.0,0.0,-1.0,0.0 } 
};

void drawCube(DrawElement *dw) {
//#pragma omp parallel for
	for (int i = 0; i < 12; i++) {
		point p1;
		point p2;
		point p3;
		int index0 = i * 3;
		int index1 = i * 3 + 1;
		int index2 = i * 3 + 2;
		p1.setPoint(vertexData[index0][0], vertexData[index0][1], vertexData[index0][2], 1.0f, 1.0f, 1.0f, 1.0f, vertexData[index0][3], vertexData[index0][4], vertexData[index0][5], vertexData[index0][6], vertexData[index0][7], 1);
		p2.setPoint(vertexData[index1][0], vertexData[index1][1], vertexData[index1][2], 1.0f, 1.0f, 1.0f, 1.0f, vertexData[index1][3], vertexData[index1][4], vertexData[index0][5], vertexData[index0][6], vertexData[index0][7], 1);
 		p3.setPoint(vertexData[index2][0], vertexData[index2][1], vertexData[index2][2], 1.0f, 1.0f, 1.0f, 1.0f, vertexData[index2][3], vertexData[index2][4], vertexData[index0][5], vertexData[index0][6], vertexData[index0][7], 1);
		dw->drawTriangle(&p1, &p2, &p3);
	}
	if (dw->isDeferred) {
		dw->drawDeferred();
	}
}


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {
	DrawElement *dw = new DrawElement();
	dw->isDeferred = true;
	dw->initDevice();
	/* World-View matrix */
	dw->cameraPosition.setVect(0.0, 0.0, -3.0);
	dw->cameraTarget.setVect(0.0, 0.0, 0.0);
	dw->cameraUp.setVect(0.0, 1.0, 0.0);
	dw->transform.set_view_matrix(&dw->cameraPosition, &dw->cameraTarget, &dw->cameraUp);
	matr_identity(&dw->transform.world_matrix);
	dw->transform.set_transform_matrix();
	dw->loadTexture("rong2.bmp");
	dw->renderMode =TEXTURE_MODE;
	dw->lightPos.setVect(0.0f, 0.0f, -3.0f);
	dw->lightDiffuse.setVect(0.0f, 0.0f, 1.0f);
	dw->lightSpecular.setVect(1.0f, 1.0f, 1.0f);
	dw->lightAmbient.setVect(0.0f, 0.0f, 1.0f);

	vect rotateRow;
	rotateRow.setVect(1, 1, 0);
	matr tempMatrix;
	double theta = 0.001f*PI;



	QueryPerformanceFrequency(&dw->frequency_);
	matr rotateMatrix;
	while (true) {
		theta = 0.1*PI / dw->fps_;
		QueryPerformanceCounter(&dw->startTick);
		dw->clearBuffer();
		rotate_matrix(&rotateMatrix, theta, &rotateRow);
		
		tempMatrix = dw->transform.world_matrix;
		matrix_mul(&dw->transform.world_matrix, &rotateMatrix, &tempMatrix);
		dw->transform.set_transform_matrix();
		drawCube(dw);
		
		/* Count FPS */
		QueryPerformanceCounter(&dw->endTick);
		delta.QuadPart = dw->endTick.QuadPart - dw->startTick.QuadPart;
		dw->fps_ = 1.0f * dw->frequency_.QuadPart / delta.QuadPart;
		sprintf_s(fpsBuffer, 256, "FPS: %.1f", dw->fps_);
		SetBkColor(dw->hMemDC, RGB(0xD7, 0xC4, 0xBB));
		SetTextColor(dw->hMemDC, RGB(0xD0, 0x10, 0x4C));
		TextOut(dw->hMemDC, 0, 0, fpsBuffer, lstrlen(fpsBuffer));
		SelectObject(dw->hMemDC, dw->hBITMAP);
		BitBlt(dw->hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, dw->hMemDC, 0, 0, SRCCOPY);
		dw->updateScene();
	}
	dw->destoryDevice();
	delete dw;
	return 0;

}