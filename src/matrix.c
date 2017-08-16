#include <string.h>

#include "SDL2/SDL.h"

#include "matrix.h"
#include "types.h"


void LoadIdentity_M3x3(TMatrix3x3 matrix)
{
    memset(matrix, 0, sizeof(TMatrix3x3));
    matrix[0] = 1.0f;
    matrix[4] = 1.0f;
    matrix[8] = 1.0f;
}



void LoadIdentity_M4x4(TMatrix4x4 matrix)
{
    memset(matrix, 0, sizeof(TMatrix4x4));
    matrix[0]  = 1.0f;
    matrix[5]  = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
}

inline void Multiplyf_Scalar(TMatrix4x4 matrix, float scalar)
{
    for (uint8 i = 0; i < 16; ++i)
        matrix[i] *= scalar;
}

void Multiplyf_M4x4(TMatrix4x4 dst, const TMatrix4x4 src1, const TMatrix4x4 src2)
{
    TMatrix4x4 temp;
    uint8 i;

	for (uint8 col = 0; col < 4; col++)
	{
		for (uint8 row = 0; row < 4; row++)
		{
			temp[col * 4 + row] = 0.0f;

			for (i = 0; i < 4; i++)
                temp[col * 4 + row] += src1[i * 4 + row] * src2[col * 4 + i];
		}
	}

	memcpy(dst, temp, sizeof(TMatrix4x4));
}

void Translatef_M4x4(TMatrix4x4 matrix, float x, float y, float z)
{
    TMatrix4x4 temp;

    LoadIdentity_M4x4(temp);

    temp[12] = x;
    temp[13] = y;
    temp[14] = z;

    Multiplyf_M4x4(matrix, matrix, temp);
}

void Translatev_M4x4(TMatrix4x4 matrix, SVector3f translate)
{
    TMatrix4x4 temp;

    LoadIdentity_M4x4(temp);

    temp[12] = translate.x;
    temp[13] = translate.y;
    temp[14] = translate.z;

    Multiplyf_M4x4(matrix, matrix, temp);
}


void Scalef_M4x4(TMatrix4x4 matrix, float x, float y, float z)
{
    TMatrix4x4 temp;

    LoadIdentity_M4x4(temp);

    temp[0]  = x;
    temp[5]  = y;
    temp[10] = z;

    Multiplyf_M4x4(matrix, matrix, temp);
}

void Scalev_M4x4(TMatrix4x4 matrix, SVector3f scale)
{
    TMatrix4x4 temp;

    LoadIdentity_M4x4(temp);

    temp[0]  = scale.x;
    temp[5]  = scale.y;
    temp[10] = scale.z;

    Multiplyf_M4x4(matrix, matrix, temp);
}

void Rotatef_M4x4(TMatrix4x4 matrix, float angle, float x, float y, float z)
{
    TMatrix4x4 temp;

    float s = SDL_sinf(angle);
    float c = SDL_cosf(angle);

    SVector3f vector = NormalizeVector3f(NewVector3f(x, y, z));

    float xn = vector.x;
    float yn = vector.y;
    float zn = vector.z;

    LoadIdentity_M4x4(temp);

    temp[0]  = xn * xn * (1 - c) + c;
    temp[1]  = xn * yn * (1 - c) + zn * s;
    temp[2]  = xn * zn * (1 - c) - yn * s;

    temp[4]  = xn * yn * (1 - c) - zn * s;
    temp[5]  = yn * yn * (1 - c) + c;
    temp[6]  = yn * zn * (1 - c) + xn * s;

    temp[8]  = xn * zn * (1 - c) + yn * s;
    temp[9]  = yn * zn * (1 - c) - xn * s;
    temp[10] = zn * zn * (1 - c) + c;

    Multiplyf_M4x4(matrix, matrix, temp);
}

void RotateRxf_M4x4(TMatrix4x4 matrix, float angle)
{
    TMatrix4x4 temp;

    float s = SDL_sinf(angle);
    float c = SDL_cosf(angle);

    LoadIdentity_M4x4(temp);

    temp[5]  = c;
    temp[6]  = s;

    temp[9]  = -s;
    temp[10] = c;

    Multiplyf_M4x4(matrix, matrix, temp);
}

void RotateRyf_M4x4(TMatrix4x4 matrix, float angle)
{
    TMatrix4x4 temp;

    float s = SDL_sinf(angle);
    float c = SDL_cosf(angle);

    LoadIdentity_M4x4(temp);

    temp[0] = c;
    temp[2] = -s;

    temp[8] = s;
    temp[10] = c;

    Multiplyf_M4x4(matrix, matrix, temp);
}

void RotateRzf_M4x4(TMatrix4x4 matrix, float angle)
{
    TMatrix4x4 temp;

    float s = SDL_sinf(angle);
    float c = SDL_cosf(angle);

    LoadIdentity_M4x4(temp);

    temp[0] = c;
    temp[1] = s;

    temp[4] = -s;
    temp[5] = c;

    Multiplyf_M4x4(matrix, matrix, temp);
}



// "camera"
bool Orthof_M4x4(TMatrix4x4 result, const float left, const float right, const float bottom, const float top, const float near, const float far)
{
	if ((right - left) == 0.0f ||
        (top - bottom) == 0.0f ||
        (far - near) == 0.0f)
	{
		return false;
	}

	memset(result, 0, 16 * sizeof(float));

    result[0]  =  2.0f / (right - left);
    result[5]  =  2.0f / (top - bottom);
    result[10] = -2.0f / (far - near);
    result[12] = -(right + left) / (right - left);
    result[13] = -(top + bottom) / (top - bottom);
    result[14] = -(far + near)   / (far - near);
    result[15] =  1.0f;

    return true;
}

bool Frustumf_M4x4(TMatrix4x4 result, const float left, const float right, const float bottom, const float top, const float near, const float far)
{
	if ((right - left) == 0.0f ||
        (top - bottom) == 0.0f ||
        (far - near) == 0.0f)
	{
		return false;
	}

	memset(result, 0, 16 * sizeof(float));

	result[0]  =  2.0f * near / (right - left);
    result[5]  =  2.0f * near / (top - bottom);
    result[8]  =  (right + left)  / (right - left);
    result[9]  =  (top + bottom)  / (top - bottom);
    result[10] = -(far + near) / (far - near);
    result[11] = -1.0f;
    result[14] = -(2.0f * far * near) / (far - near);

    return true;
}

bool Perspectivef_M4x4(TMatrix4x4 result, const float fovy, const float aspect, const float zNear, const float zFar)
{
    if (fovy <= 0.0f || fovy >= 180.0f)
    {
    	return false;
    }

    float xmin, xmax, ymin, ymax;
    ymax =  zNear * SDL_tanf(fovy * M_PI / 360.0f);
    ymin = -ymax;
    xmin =  ymin * aspect;
    xmax =  ymax * aspect;

    return Frustumf_M4x4(result, xmin, xmax, ymin, ymax, zNear, zFar);
}

void LookAtf_M4x4(TMatrix4x4 result, const float eyeX, const float eyeY, const float eyeZ, const float centerX, const float centerY, const float centerZ, const float upX, const float upY, const float upZ)
{
    SVector3f forward, side, up;

    forward.x = centerX - eyeX;
    forward.y = centerY - eyeY;
    forward.z = centerZ - eyeZ;

    forward = NormalizeVector3f(forward);

    up.x = upX;
    up.y = upY;
    up.z = upZ;

    side = CrossVector3f(forward, up);
    side = NormalizeVector3f(side);

    up = CrossVector3f(side, forward);

	memset(result, 0, 16 * sizeof(float));

    result[0] =  side.x;
    result[1] =  up.x;
    result[2] = -forward.x;
    result[4] =  side.y;
    result[5] =  up.y;
    result[6] = -forward.y;
    result[8] =  side.z;
    result[9] =  up.z;
    result[10] = -forward.z;
    result[15] = 1.0f;

    Translatef_M4x4(result, -eyeX, -eyeY, -eyeZ);
}

inline void LookAtv_M4x4(TMatrix4x4 result, const SVector3f eye, const SVector3f center, const SVector3f up)
{
    LookAtf_M4x4(result, eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
}

