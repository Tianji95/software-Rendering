#include"DrawElement.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb_image.h"
#include"stb_image_write.h"
Scanline::Scanline() {
	this->y = 0;
	this->x = 0;
	this->width = 0;

}

Scanline::~Scanline() {
}

Trapezoid::Trapezoid() {
	this->top = 0;
	this->bottom = 0;
}

Trapezoid::~Trapezoid() {
}

DrawElement::DrawElement() {
	bmpBuffer_ = nullptr;
	zBuffer_ = nullptr;
}

DrawElement::~DrawElement() {
	this->destoryDevice();
}

HRESULT DrawElement::initDevice()
{
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = hinstSelf;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOWS_CLASS_NAME;
	wcex.hIconSm = LoadIcon(hinstSelf, IDI_APPLICATION);
	RegisterClassEx(&wcex);
	hwnd = CreateWindowEx(
		0,
		WINDOWS_CLASS_NAME,
		"SoftRendering",
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		GetModuleHandle(nullptr),
		this
	);

	HRESULT hr = S_OK;
	hdc = GetDC(hwnd);//根据窗体句柄得到窗体中的设备描述句柄
	hMemDC = CreateCompatibleDC(hdc);//根据设备描述句柄得到一个兼容的设备描述句柄
	hBITMAP = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //根据设备描述句柄得到一个位图句柄
	SelectObject(hMemDC, hBITMAP);
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = WINDOW_WIDTH;
	bmpInfo.bmiHeader.biHeight = -WINDOW_HEIGHT;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32; 
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;
	hBITMAP = CreateDIBSection(hMemDC, &bmpInfo, DIB_RGB_COLORS, (void**)&bmpBuffer_, NULL, 0);//创建一个可以直接写的buffer
	zBuffer_ = new double[WINDOW_HEIGHT * WINDOW_WIDTH];
	int i;
	for (i = 0; i < WINDOW_HEIGHT * WINDOW_WIDTH; i++) {
		zBuffer_[i]= -100;
	}
	/*Z Buffer and Texture*/

	/* set Projection parameters */
	transform.set_projection_matrix(0.33f*PI, (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT, 1.0f, 100.0f);
	transform.set_transform_matrix();
	/* Light */
	/* FPS counter */
	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);
	return hr;
}


void DrawElement::clearBuffer() {
	int i;
	for (i = 0; i < WINDOW_HEIGHT * WINDOW_WIDTH; i++) {
		this->bmpBuffer_[i] = 0x00000000;
		zBuffer_[i] = -100;
	}
}


void DrawElement::destoryDevice() {
	ReleaseDC(hwnd, hMemDC);
}

LRESULT DrawElement::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case 104:
			break;
		case 105:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void DrawElement::drawTriangle(point* v1, point* v2, point*v3) {
	vect transV1, transV2,transV3;
	vect screenV1, screenV2, screenV3;
	vect worldnormalV1, worldnormalV2, worldnormalV3;
	vect worldV1, worldV2, worldV3;

	vector_mul_matrix(&transV1, &this->transform.transform_matrix, &v1->pos);
	vector_mul_matrix(&transV2, &this->transform.transform_matrix, &v2->pos);
	vector_mul_matrix(&transV3, &this->transform.transform_matrix, &v3->pos);

	if (!checkCvv(&transV1) && !checkCvv(&transV2) && !checkCvv(&transV3)) {
		return;//判断是否在裁剪空间里面
	}

	vector_mul_matrix(&worldV1, &this->transform.world_matrix, &v1->pos);
	vector_mul_matrix(&worldV2, &this->transform.world_matrix, &v2->pos);
	vector_mul_matrix(&worldV3, &this->transform.world_matrix, &v3->pos);

	vector_mul_matrix(&worldnormalV1, &this->transform.world_matrix, &v1->nvect);
	vector_mul_matrix(&worldnormalV2, &this->transform.world_matrix, &v2->nvect);
	vector_mul_matrix(&worldnormalV3, &this->transform.world_matrix, &v3->nvect);

	if (vect_dotmul(&worldnormalV1, &this->cameraPosition) < 0) {
		return;
	}

	getScreenPos(&screenV1, &transV1, WINDOW_WIDTH, WINDOW_HEIGHT);//获取屏幕坐标
	getScreenPos(&screenV2, &transV2, WINDOW_WIDTH, WINDOW_HEIGHT);
	getScreenPos(&screenV3, &transV3, WINDOW_WIDTH, WINDOW_HEIGHT);

	point finalV1, finalV2, finalV3;
	finalV1.setPoint(screenV1.x, screenV1.y, screenV1.z, 1.0f, v1->r, v1->g, v1->b, v1->u, v1->v, worldnormalV1.x, worldnormalV1.y, worldnormalV1.z, transV1.w);
	finalV2.setPoint(screenV2.x, screenV2.y, screenV2.z, 1.0f, v2->r, v2->g, v2->b, v2->u, v2->v, worldnormalV2.x, worldnormalV2.y, worldnormalV2.z, transV2.w);
	finalV3.setPoint(screenV3.x, screenV3.y, screenV3.z, 1.0f, v3->r, v3->g, v3->b, v3->u, v3->v, worldnormalV3.x, worldnormalV3.y, worldnormalV3.z, transV3.w);

	
	Trapezoid trape1;
	Trapezoid trape2;
	int n = divideTriangle(&finalV1, &finalV2, &finalV3, &trape1, &trape2, &worldV1, &worldV2, &worldV3);

	if(n >= 1){
		randerTriangle(&trape1);
	}
	if (n == 2) {
		randerTriangle(&trape2);
	}
}

void DrawElement::randerTriangle(Trapezoid* trape) {
	int firstLine= max((int)trape->top, 0);
	int lastLine = min((int)trape->bottom, WINDOW_HEIGHT - 1);
//#pragma omp parallel for
	for (int h_idx = firstLine; h_idx < lastLine; h_idx++) {
		Scanline scline;
		initScanLine(&scline, h_idx, trape);
		drawScanLine(&scline);
	}
}

int DrawElement::divideTriangle(point* finalV1, point* finalV2, point* finalV3, Trapezoid* trape1, Trapezoid* trape2, vect* oriV1, vect* oriV2, vect* oriV3) {
	point *v1 = finalV1;
	point *v2 = finalV2;
	point *v3 = finalV3;
	vect *ori1 = oriV1;
	vect *ori2 = oriV2;
	vect *ori3 = oriV3;
	point *temp;
	vect *oriTemp;

	if (v1->pos.y > v2->pos.y) {
		temp = v1;
   		v1 = v2;
		v2 = temp;
		oriTemp = ori1;
		ori1 = ori2;
		ori2 = oriTemp;
	}
	if (v1->pos.y > v3->pos.y) {
		temp = v1;
		v1 = v3;
		v3 = temp;
		oriTemp = ori1;
		ori1 = ori3;
		ori3 = oriTemp;
	}
	if (v2->pos.y > v3->pos.y) {
		temp = v2;
		v2 = v3;
		v3 = temp;
		oriTemp = ori3;
		ori3 = ori2;
		ori2 = oriTemp;
	}

	
	//如果v1和v2直线平行于x（倒三角）
	if (v1->pos.y == v2->pos.y) {
		//v1必须在v2左边，否则需要交换一下
		if (v1->pos.x > v2->pos.x) {
			temp = v1;
			v1 = v2;
			v2 = temp;
			oriTemp = ori1;
			ori1 = ori2;
			ori2 = oriTemp;
		}
		trape1->top = v1->pos.y;
		trape1->bottom = v3->pos.y;
		trape1->left1 = *v1;
		trape1->left2 = *v3;
		trape1->right1 = *v2;
		trape1->right2 = *v3;

		trape1->oriLeft1 = *ori1;
		trape1->oriLeft2 = *ori3;
		trape1->oriRight1 = *ori2;
		trape1->oriRight2 = *ori3;
		return 1;
	}
	//如果是正三角
	if (v2->pos.y == v3->pos.y) {
		if (v2->pos.x > v3->pos.x) {
			temp = v2;
			v2 = v3;
			v3 = temp;
			oriTemp = ori3;
			ori3 = ori2;
			ori2 = oriTemp;
		}
		trape1->top = v1->pos.y;
		trape1->bottom = v3->pos.y;
		trape1->left1 = *v1;
		trape1->left2 = *v2;
		trape1->right1 = *v1;
		trape1->right2 = *v3;

		trape1->oriLeft1 = *ori1;
		trape1->oriLeft2 = *ori2;
		trape1->oriRight1 = *ori1;
		trape1->oriRight2 = *ori3;
		return 1;
	}
	trape1->top = v1->pos.y;
	trape1->bottom = v2->pos.y;
	trape2->top = v2->pos.y;
	trape2->bottom = v3->pos.y;

	double k = (v3->pos.y - v1->pos.y) / (v2->pos.y - v1->pos.y);
	double invk = 1.0f / k;
	double v_compare = v1->pos.x + (v2->pos.x - v1->pos.x) * k;
	point point_middle;
	vect ori_vect_middle;
	vect_interp(&ori_vect_middle, ori1, ori3, invk);
	point_interp(&point_middle, v1, v3, invk);
	//说明v1和v2线在v1和v3线的左边
	if (v_compare <= v3->pos.x) {
		trape1->left1 = *v1;
		trape1->left2 = *v2;
		trape1->right1 = *v1;
		trape1->right2 = point_middle;

		trape1->oriLeft1 = *ori1;
		trape1->oriLeft2 = *ori2;
		trape1->oriRight1 = *ori1;
		trape1->oriRight2 = ori_vect_middle;

		trape2->left1 = *v2;
		trape2->left2 = *v3;
		trape2->right1 = point_middle;
		trape2->right2 = *v3;

		trape2->oriLeft1 = *ori2;
		trape2->oriLeft2 = *ori3;
		trape2->oriRight1 = ori_vect_middle;
		trape2->oriRight2 = *ori3;
	}
	else {
		trape1->left1 = *v1;
		trape1->left2 = point_middle;
		trape1->right1 = *v1;
		trape1->right2 = *v2;

		trape1->oriLeft1 = *ori1;
		trape1->oriLeft2 = ori_vect_middle;
		trape1->oriRight1 = *ori1;
		trape1->oriRight2 = *ori2;

		trape2->left1 = point_middle;
		trape2->left2 = *v3;
		trape2->right1 = *v2;
		trape2->right2 = *v3;

		trape2->oriLeft1 = ori_vect_middle;
		trape2->oriLeft2 = *ori3;
		trape2->oriRight1 = *ori2;
		trape2->oriRight2 = *ori3;
	}
	return 2;
}


void DrawElement::initScanLine(Scanline *scline, int yPos, Trapezoid* trape) {
	//计算扫描线的左右两个顶点
	point leftP;
	point rightP;
	vect oriLeftP;
	vect oriRightP;

	double leftlen = trape->left1.pos.y - trape->left2.pos.y;
	double interp_pos = (trape->left1.pos.y - yPos) / leftlen;
	point_interp(&leftP, &trape->left1, &trape->left2, interp_pos);
	point_interp(&rightP,&trape->right1, &trape->right2, interp_pos);
	vect_interp(&oriLeftP, &trape->oriLeft1, &trape->oriLeft2, interp_pos);
	vect_interp(&oriRightP, &trape->oriRight1, &trape->oriRight2, interp_pos);

	scline->x = (int)(leftP.pos.x + 0.5f);
	scline->width = (int)(rightP.pos.x + 0.5f) - scline->x;
	scline->y = yPos;
	scline->leftPoint = leftP;
	scline->rightPoint = rightP;
	scline->oriLeftVect = oriLeftP;
	scline->oriRightVect = oriRightP;
	double step;
	//防止每次都要计算step，这样就只需要计算一次了
	if (scline->width != 0) {
		step = 1.0f / scline->width;
	}
 	else {
		step = 0;
	}
	scline->oriStepVect.x = (oriRightP.x - oriLeftP.x)*step;
	scline->oriStepVect.y = (oriRightP.y - oriLeftP.y)*step;
	scline->oriStepVect.z = (oriRightP.z - oriLeftP.z)*step;
	scline->stepPoint.pos.x = (rightP.pos.x - leftP.pos.x)*step;
	scline->stepPoint.pos.y = (rightP.pos.y - leftP.pos.y)*step;
	scline->stepPoint.pos.z = (rightP.pos.z - leftP.pos.z)*step;
	scline->stepPoint.pos.w = (rightP.pos.w - leftP.pos.w)*step;
	scline->stepPoint.r = (rightP.r - leftP.r)*step;
	scline->stepPoint.g = (rightP.g - leftP.g)*step;
	scline->stepPoint.b = (rightP.b - leftP.b)*step;
	scline->stepPoint.u = (rightP.u - leftP.u)*step;
	scline->stepPoint.v = (rightP.v - leftP.v)*step;

}


void DrawElement::drawScanLine(Scanline *scline) {
	int xPos = scline->x;
	int widthPos = 0;
	point nowPoint;
	vect nowOriginPos;
	vect pointToCamera, pointToLight;
	vect tempH, H;
	vect normal;
	int texture_xPos;
	int texture_yPos;
	double lightR, lightG, lightB;
	double lightshini;
	int position;
	int R, G, B;
	double sclinelen = scline->leftPoint.pos.x - scline->rightPoint.pos.x;
	nowPoint = scline->leftPoint;
	nowOriginPos = scline->oriLeftVect;
	vect_normalize(&normal, &scline->leftPoint.nvect);

		

	for (; widthPos < scline->width && xPos < WINDOW_WIDTH; widthPos++, xPos++) {
		double z = nowPoint.pos.z;
		int pointPos = scline->y*WINDOW_WIDTH + xPos;
		
		if (z > this->zBuffer_[pointPos]) {
			this->zBuffer_[pointPos] = z;
			//消耗70-80fps
			tempH.setVect(this->cameraPosition.x - nowOriginPos.x, this->cameraPosition.y - nowOriginPos.y, this->cameraPosition.z - nowOriginPos.z);
			vect_normalize(&pointToCamera, &tempH);
			tempH.setVect(this->lightPos.x - nowOriginPos.x, this->lightPos.y - nowOriginPos.y, this->lightPos.z - nowOriginPos.z);
			vect_normalize(&pointToLight, &tempH);
			tempH.setVect((pointToLight.x + pointToCamera.x)*0.5f, (pointToLight.y + pointToCamera.y)*0.5f, (pointToLight.z + pointToCamera.z)*0.5f);
			vect_normalize(&H, &tempH);

			//消耗10fps
			double maxdot = max(vect_dotmul(&normal, &H), 0);
			maxdot = maxdot*maxdot;
			lightshini = this->ks * maxdot * maxdot;
			lightR = this->lightSpecular.x * lightshini;
			lightG = this->lightSpecular.y * lightshini;
			lightB = this->lightSpecular.z * lightshini;

			if (this->renderMode == COLOR_MODE) {
				R = (int)(nowPoint.r * 255.0f*ke);
				G = (int)(nowPoint.g * 255.0f*ke);
				B = (int)(nowPoint.b * 255.0f*ke);
				this->bmpBuffer_[pointPos] = ((int)(R * lightR) << 16) | ((int)(G*lightG)<< 8) | (int)(B*lightB);
			}
			else if (this->renderMode == TEXTURE_MODE) {
				texture_xPos = (int)(nowPoint.u * (this->textureWidth-1) + 0.5f);
				texture_yPos = (int)(nowPoint.v * (this->textureHeight-1) + 0.5f);
				position = 3 * (texture_yPos * this->textureWidth + texture_xPos);
				R = this->textureBuffer[position];
				G = this->textureBuffer[position + 1];
				B = this->textureBuffer[position + 2];
				this->bmpBuffer_[pointPos] = ((int)(R * lightR) << 16) | ((int)(G*lightG) << 8) | (int)(B*lightB);
			}
		}
		nowOriginPos.x += scline->oriStepVect.x;
		nowOriginPos.y += scline->oriStepVect.y;
		nowOriginPos.z += scline->oriStepVect.z;
		nowPoint.pos.x += scline->stepPoint.pos.x;
		nowPoint.pos.y += scline->stepPoint.pos.y;
		nowPoint.pos.z += scline->stepPoint.pos.z;
		nowPoint.r += scline->stepPoint.r;
		nowPoint.g += scline->stepPoint.g;
		nowPoint.b += scline->stepPoint.b;

		if (scline->leftPoint.pos.z == scline->rightPoint.pos.z) {
			nowPoint.u += scline->stepPoint.u;
			nowPoint.v += scline->stepPoint.v;
		}
		else {
			double interp_pos = (scline->leftPoint.pos.x - nowPoint.pos.x) / sclinelen;
			nowPoint.pos.z = (scline->leftPoint.pos.z*scline->rightPoint.pos.z) / ((scline->leftPoint.pos.z - scline->rightPoint.pos.z)*interp_pos + scline->rightPoint.pos.z);
			double rate = nowPoint.pos.z / (scline->leftPoint.pos.z*scline->rightPoint.pos.z);
			nowPoint.u = (scline->leftPoint.u*scline->rightPoint.pos.z*(1.0f - interp_pos) + interp_pos*scline->rightPoint.u*scline->leftPoint.pos.z)*rate;
			nowPoint.v = (scline->leftPoint.v*scline->rightPoint.pos.z*(1.0f - interp_pos) + interp_pos*scline->rightPoint.v*scline->leftPoint.pos.z)*rate;
		}
	}
}

void DrawElement::updateScene() {
	HDC hDC = GetDC(hwnd);
	SelectObject(hMemDC, hBITMAP);
	BitBlt(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, SRCCOPY);
	//ReleaseDC(hwnd, hDC);
	//ReleaseDC(hwnd, hMemDC);
}

void DrawElement::loadTexture(char* filename) {
	unsigned char *data;
	this->textureBuffer = stbi_load(filename, &this->textureWidth, &this->textureHeight, &this->textureChannal, 3);
}