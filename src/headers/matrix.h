/*
	It's part of bodhi GameEngine.
    Copyright (C) 2017 Anton "Vuvk" Shcherbatykh

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "vector.h"

typedef float TMatrix3x3[9];
typedef float TMatrix4x4[16];



void LoadIdentity_M3x3(TMatrix3x3 matrix);



/* matrix 4x4 operations */
void LoadIdentity_M4x4(TMatrix4x4 matrix);

void Multiplyf_Scalar(TMatrix4x4 matrix, float scalar);
void Multiplyf_M4x4(TMatrix4x4 dst, const TMatrix4x4 src1, const TMatrix4x4 src2);

void Translatef_M4x4(TMatrix4x4 matrix, float x, float y, float z);
void Translatev_M4x4(TMatrix4x4 matrix, SVector3f translate);

void Scalef_M4x4(TMatrix4x4 matrix, float x, float y, float z);
void Scalev_M4x4(TMatrix4x4 matrix, SVector3f scale);

void Rotatef_M4x4(TMatrix4x4 matrix, float angle, float x, float y, float z);
void RotateRxf_M4x4(TMatrix4x4 matrix, float angle);
void RotateRyf_M4x4(TMatrix4x4 matrix, float angle);
void RotateRzf_M4x4(TMatrix4x4 matrix, float angle);


/* "camera" */
bool Orthof_M4x4(TMatrix4x4 result, const float left, const float right, const float bottom, const float top, const float near, const float far);
bool Frustumf_M4x4(TMatrix4x4 result, const float left, const float right, const float bottom, const float top, const float near, const float far);
bool Perspectivef_M4x4(TMatrix4x4 result, const float fovy, const float aspect, const float zNear, const float zFar);
void LookAtf_M4x4(TMatrix4x4 result, const float eyeX, const float eyeY, const float eyeZ, const float centerX, const float centerY, const float centerZ, const float upX, const float upY, const float upZ);
void LookAtv_M4x4(TMatrix4x4 result, const SVector3f eye, const SVector3f center, const SVector3f up);
