#include "Matrix.h"
#include <cmath>

Matrix::Matrix(Matrix & Mat)
{
	for(int i = 0; i < 16; i++)
		MatrixData[i] = Mat.MatrixData[i];
}

Matrix::Matrix(Matrix && Mat)
{
	for(int i = 0; i < 16; i++)
		MatrixData[i] = Mat.MatrixData[i];
}

Matrix::Matrix(const float * InitData)
{
	for(int i = 0; i < 16; i++)
		MatrixData[i] = InitData[i];
}

Matrix::Matrix(float Factor, InitParam w)
{
	switch(w)
	{
	case ROTATE_X:
		InitRotation_x(Factor);
		break;

	case ROTATE_Y:
		InitRotation_y(Factor);
		break;

	case ROTATE_Z:
		InitRotation_z(Factor);
		break;
	}
}

Matrix Matrix::operator *(Matrix & Mat)
{
	Matrix Temp;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			Temp.MatrixData[i*4 + j] =  MatrixData[i*4]*Mat.MatrixData[j*4]+
										MatrixData[i*4+1]*Mat.MatrixData[j*4+1]+
										MatrixData[i*4+2]*Mat.MatrixData[j*4+2]+
										MatrixData[i*4+3]*Mat.MatrixData[j*4+3];
	return Temp;
}

Matrix Matrix::operator *(Matrix && Mat)
{
	Matrix Temp;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			Temp.MatrixData[i*4 + j] =  MatrixData[j]*Mat.MatrixData[i*4]+
										MatrixData[j+4]*Mat.MatrixData[i*4+1]+
										MatrixData[j+8]*Mat.MatrixData[i*4+2]+
										MatrixData[j+12]*Mat.MatrixData[i*4+3];
	return Temp;
}

Matrix & Matrix::operator =(Matrix & Mat)
{
	for(int i = 0; i < 16; i++)
		MatrixData[i] = Mat.MatrixData[i];
	return *this;
}

Matrix & Matrix::operator =(Matrix && Mat)
{
	for(int i = 0; i < 16; i++)
		MatrixData[i] = Mat.MatrixData[i];
	return *this;
}


Matrix & Matrix::Clear()
{
	MatrixData[0] = MatrixData[5] = MatrixData[10] = MatrixData[15] = 1.0f;
	MatrixData[1] = MatrixData[4] = MatrixData[8] = MatrixData[12] = 0.0f;
	MatrixData[2] = MatrixData[6] = MatrixData[9] = MatrixData[13] = 0.0f;
	MatrixData[3] = MatrixData[7] = MatrixData[11] = MatrixData[14] = 0.0f;
	return *this;
}

Matrix & Matrix::InitTranslation(float x, float y, float z)
{
	Clear();
	MatrixData[12] = x;
	MatrixData[13] = y;
	MatrixData[14] = z;
	return *this;
}

Matrix & Matrix::InitRotation_x(float Angle)
{
	Clear();
	MatrixData[5] = std::cos(Angle);
	MatrixData[6] = -std::sin(Angle);
	MatrixData[9] = std::sin(Angle);
	MatrixData[10] = std::cos(Angle);
	return *this;
}

Matrix & Matrix::InitRotation_y(float Angle)
{
	Clear();
	MatrixData[0] = std::cos(Angle);
	MatrixData[2] = std::sin(Angle);
	MatrixData[8] = -std::sin(Angle);
	MatrixData[10] = std::cos(Angle);
	return *this;
}

Matrix & Matrix::InitRotation_z(float Angle)
{
	Clear();
	MatrixData[0] = std::cos(Angle);
	MatrixData[1] = std::sin(Angle);
	MatrixData[4] = -std::sin(Angle);
	MatrixData[5] = std::cos(Angle);
	return *this;
}

Matrix & Matrix::InitScale(float Factor)
{
	Clear();
	MatrixData[0] = Factor;
	MatrixData[5] = Factor;
	MatrixData[10] = Factor;
	return *this;
}

Matrix & Matrix::InitFrustum(float NearPlane, float FarPlane, float Left, float Right, float Bottom, float Top)
{
	Clear();
	MatrixData[0] = (2.0f*NearPlane)/(Right-Left);
	MatrixData[5] = (2.0f*NearPlane)/(Top-Bottom);
	MatrixData[8] = (Right+Left)/(Right-Left);
	MatrixData[9] = (Top+Bottom)/(Top-Bottom);
	MatrixData[10] = (NearPlane+FarPlane)/(NearPlane-FarPlane);
	MatrixData[11] = -1.0f;
	MatrixData[14] = (2.0f*NearPlane*FarPlane)/(NearPlane-FarPlane);
	return *this;
}

Matrix & Matrix::InitFrustum(float NearPlane, float FarPlane, float Fov, float AspectRatio)
{
	float Top = std::tan(Fov/2.0f)*NearPlane;
	float Right = Top*AspectRatio;
	InitFrustum(NearPlane, FarPlane, -Right, Right, -Top, Top);
	return *this;
}

