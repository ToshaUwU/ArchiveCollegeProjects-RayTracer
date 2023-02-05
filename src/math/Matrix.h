#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

class Matrix
{
private:

	float MatrixData[16];

public:
	Matrix(Matrix & Mat);
	Matrix(Matrix && Mat);
	Matrix(const float * InitData);
	Matrix(){Clear();}
	Matrix(float x, float y, float z){InitTranslation(x, y, z);}
	Matrix(float ScaleFactor){InitScale(ScaleFactor);}
	enum InitParam{ROTATE_X, ROTATE_Y, ROTATE_Z};
	Matrix(float Factor, InitParam w);
	Matrix(float NearPlane, float FarPlane, float Left, float Right, float Bottom, float Top){InitFrustum(NearPlane, FarPlane, Left, Right, Bottom, Top);}
	Matrix(float NearPlane, float FarPlane, float Fov, float AspectRatio){InitFrustum(NearPlane, FarPlane, Fov, AspectRatio);}

	Matrix operator *(Matrix & Mat);
	Matrix operator *(Matrix && Mat);
	Matrix & operator =(Matrix & Mat);
	Matrix & operator =(Matrix && Mat);

	operator float *(){return MatrixData;}

	Matrix & Clear();
	Matrix & InitTranslation(float x, float y, float z);
	Matrix & InitRotation_x(float Angle);
	Matrix & InitRotation_y(float Angle);
	Matrix & InitRotation_z(float Angle);
	Matrix & InitScale(float Factor);
	Matrix & InitFrustum(float NearPlane, float FarPlane, float Left, float Right, float Bottom, float Top);
	Matrix & InitFrustum(float NearPlane, float FarPlane, float Fov, float AspectRatio);

	~Matrix() = default;
};

inline Matrix CreateScaleMatrix(float Factor){return Matrix(Factor);}
inline Matrix CreateTranslationMatrix(float x, float y, float z){return Matrix(x, y, z);}
inline Matrix CreateFrustumMatrix(float NearPlane, float FarPlane, float Left, float Right, float Bottom, float Top){return Matrix(NearPlane, FarPlane, Left, Right, Bottom, Top);}
inline Matrix CreateFrustumMatrix(float NearPlane, float FarPlane, float Fov, float AspectRatio){return Matrix(NearPlane, FarPlane, Fov, AspectRatio);}
inline Matrix CreateXRotationMatrix(float Angle){return Matrix(Angle, Matrix::ROTATE_X);}
inline Matrix CreateYRotationMatrix(float Angle){return Matrix(Angle, Matrix::ROTATE_Y);}
inline Matrix CreateZRotationMatrix(float Angle){return Matrix(Angle, Matrix::ROTATE_Z);}

#endif // MATRIX_H_INCLUDED
