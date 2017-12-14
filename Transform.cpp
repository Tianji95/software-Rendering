#include"Transform.h"
#include<cmath>

Transform::Transform() {
	width = height = 0.0;
}

Transform::~Transform() {
	width = height = 0;
}


//返回一个右手坐标系的透视投影矩阵，类似D3DXMatrixPerspectiveFovRH,参考https://msdn.microsoft.com/en-us/library/windows/desktop/bb205351(v=vs.85).aspx
//fovy：观察时y轴方向的角度（弧度）
//aspect：纵横比，视线空间的宽度除以高度
//zn:近裁剪面的Z值
//zf：远裁剪面的Z值
void Transform::set_projection_matrix(double fovy, double aspect, double zn ,double zf) {
	double theCot = 1.0f / tan(fovy / 2.0f);
	this->projection_matrix.m[0][0] = theCot / aspect;
	this->projection_matrix.m[1][1] = theCot;
	this->projection_matrix.m[2][2] = zf / (zf - zn);
	this->projection_matrix.m[2][3] = -1;
	this->projection_matrix.m[3][2] = zn*zf / (zf - zn);
}

//视角变换矩阵，类似于LookAt，参考https://msdn.microsoft.com/en-us/library/windows/desktop/bb281711(v=vs.85).aspx
//cameraposition : 摄像机的位置
//cameraTarget：摄像机朝向的方向
//up：摄像机向上的方向

void Transform::set_view_matrix(vect* cameraPosition, vect* cameraTarget, vect* up) {
	vect zaxis, xaxis, yaxis, subVect;
	vect_sub(&subVect, cameraPosition, cameraTarget);
	vect_normalize(&zaxis, &subVect);

	vect_crossmul(&subVect, up, &zaxis);
	vect_normalize(&xaxis, &subVect);
	vect_crossmul(&yaxis, &zaxis, &xaxis);

	this->view_matrix.m[0][0] = xaxis.x;
	this->view_matrix.m[0][1] = yaxis.x;
	this->view_matrix.m[0][2] = zaxis.x;
	this->view_matrix.m[0][3] = 0;

	this->view_matrix.m[1][0] = xaxis.y;
	this->view_matrix.m[1][1] = yaxis.y;
	this->view_matrix.m[1][2] = zaxis.y;
	this->view_matrix.m[1][3] = 0;

	this->view_matrix.m[2][0] = xaxis.z;
	this->view_matrix.m[2][1] = yaxis.z;
	this->view_matrix.m[2][2] = zaxis.z;
	this->view_matrix.m[2][3] = 0;

	this->view_matrix.m[3][0] = -vect_dotmul(&xaxis, cameraPosition);
	this->view_matrix.m[3][1] = -vect_dotmul(&yaxis, cameraPosition);
	this->view_matrix.m[3][2] = -vect_dotmul(&zaxis, cameraPosition);
	this->view_matrix.m[3][3] = 1;

}

//得到最终的转换矩阵
void Transform::set_transform_matrix() {
	matr tempmatr;
	matrix_mul(&tempmatr, &this->world_matrix, &this->view_matrix);
	matrix_mul(&this->transform_matrix, &tempmatr, &this->projection_matrix);
}

void getScreenPos(vect* out, vect* v_in, double width, double height) {
	double rate = 1.0f / v_in->w;
	out->x = (int)((v_in->x * rate + 1.0f) * width * 0.5f + 0.5f);
	out->y = (int)((1.0f - v_in->y * rate) * height * 0.5f + 0.5f);
	out->z = v_in->z * rate;
	out->w = 1.0f;
}
