#pragma once
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_
#include"MyMath.h"

void getScreenPos(vect* out, vect* v_in, double width, double height);
class Transform {
public:
	Transform();
	~Transform();

	matr world_matrix;
	matr projection_matrix;
	matr view_matrix;
	matr transform_matrix;
	double width;
	double height;
	
	void set_projection_matrix(double fovy, double aspect, double zn, double zf);
	void set_view_matrix(vect* cameraPosition, vect* cameraTarget, vect* up);
	void set_transform_matrix();

};




#endif // !_TRANSFORM_H_
