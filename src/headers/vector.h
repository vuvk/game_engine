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


#include "types.h"

typedef struct
{
	int32 x, y;
} SVector2i;

typedef struct
{
	float x, y;
} SVector2f;

typedef struct
{
	int16 x, y, z;
} SVector3s;

typedef struct
{
	int32 x, y, z;
} SVector3i;

typedef struct
{
	float x, y, z;
} SVector3f;

typedef struct
{
	int32 x, y, z, w;
} SVector4i;

typedef struct
{
	float x, y, z, w;
} SVector4f;


/* zero vector */
static const SVector2i vZero2i = {0, 0};
static const SVector3s vZero3s = {0, 0, 0};
static const SVector3i vZero3i = {0, 0, 0};
static const SVector4i vZero4i = {0, 0, 0, 0};

static const SVector2f vZero2f = {0.0f, 0.0f};
static const SVector3f vZero3f = {0.0f, 0.0f, 0.0f};
static const SVector4f vZero4f = {0.0f, 0.0f, 0.0f, 0.0f};

/* right vector */
static const SVector3f vRight3f = {1.0f, 0.0f, 0.0f};

/* up vector */
static const SVector3f vUp3f = {0.0f, 1.0f, 0.0f};

/* forward vector */
static const SVector3f vForward3f = {0.0f, 0.0f, 1.0f};


/* "constructors" */
SVector2i NewVector2i(int32 x, int32 y);
SVector2f NewVector2f(float x, float y);
SVector3s NewVector3s(int16 x, int16 y, int16 z);
SVector3i NewVector3i(int32 x, int32 y, int32 z);
SVector3f NewVector3f(float x, float y, float z);
SVector4i NewVector4i(int32 x, int32 y, int32 z, int32 w);
SVector4f NewVector4f(float x, float y, float z, float w);

/* transformation from .. to .. */
SVector3f Vector2fToVector3f(SVector2f vec);
SVector3f Vector4fToVector3f(SVector4f vec);
SVector4f Vector3fToVector4f(SVector3f vec);

/* equals */
bool IsEqualVector2i(SVector2i vec1, SVector2i vec2);
bool IsEqualVector2f(SVector2f vec1, SVector2f vec2);
bool IsEqualVector3s(SVector3s vec1, SVector3s vec2);
bool IsEqualVector3i(SVector3i vec1, SVector3i vec2);
bool IsEqualVector3f(SVector3f vec1, SVector3f vec2);
bool IsEqualVector4i(SVector4i vec1, SVector4i vec2);
bool IsEqualVector4f(SVector4f vec1, SVector4f vec2);

/* math operations */
/* addition */
SVector2i AddVector2i(SVector2i vec1, SVector2i vec2);
SVector2f AddVector2f(SVector2f vec1, SVector2f vec2);
SVector3s AddVector3s(SVector3s vec1, SVector3s vec2);
SVector3i AddVector3i(SVector3i vec1, SVector3i vec2);
SVector3f AddVector3f(SVector3f vec1, SVector3f vec2);
SVector4i AddVector4i(SVector4i vec1, SVector4i vec2);
SVector4f AddVector4f(SVector4f vec1, SVector4f vec2);

/* subtract */
SVector2i SubVector2i(SVector2i vec1, SVector2i vec2);
SVector2f SubVector2f(SVector2f vec1, SVector2f vec2);
SVector3s SubVector3s(SVector3s vec1, SVector3s vec2);
SVector3i SubVector3i(SVector3i vec1, SVector3i vec2);
SVector3f SubVector3f(SVector3f vec1, SVector3f vec2);
SVector4i SubVector4i(SVector4i vec1, SVector4i vec2);
SVector4f SubVector4f(SVector4f vec1, SVector4f vec2);

/* multiplication by scalar */
SVector2i MultVector2i(SVector2i vec, float scalar);
SVector2f MultVector2f(SVector2f vec, float scalar);
SVector3s MultVector3s(SVector3s vec, float scalar);
SVector3i MultVector3i(SVector3i vec, float scalar);
SVector3f MultVector3f(SVector3f vec, float scalar);
SVector4i MultVector4i(SVector4i vec, float scalar);
SVector4f MultVector4f(SVector4f vec, float scalar);

/* division by scalar */
SVector2i DivVector2i(SVector2i vec, float scalar);
SVector2f DivVector2f(SVector2f vec, float scalar);
SVector3s DivVector3s(SVector3s vec, float scalar);
SVector3i DivVector3i(SVector3i vec, float scalar);
SVector3f DivVector3f(SVector3f vec, float scalar);
SVector4i DivVector4i(SVector4i vec, float scalar);
SVector4f DivVector4f(SVector4f vec, float scalar);

/* magnitude of vector */
float LenVector2i(SVector2i vec);
float LenVector2f(SVector2f vec);
float LenVector3s(SVector3s vec);
float LenVector3i(SVector3i vec);
float LenVector3f(SVector3f vec);

/* normalize vector */
SVector2i NormalizeVector2i(SVector2i vec);
SVector2f NormalizeVector2f(SVector2f vec);
SVector3s NormalizeVector3s(SVector3s vec);
SVector3i NormalizeVector3i(SVector3i vec);
SVector3f NormalizeVector3f(SVector3f vec);

/* dot product */
float DotVector2i(SVector2i vec1, SVector2i vec2);
float DotVector2f(SVector2f vec1, SVector2f vec2);
float DotVector3s(SVector3s vec1, SVector3s vec2);
float DotVector3i(SVector3i vec1, SVector3i vec2);
float DotVector3f(SVector3f vec1, SVector3f vec2);

/* cross product */
SVector3s CrossVector3s(SVector3s vec1, SVector3s vec2);
SVector3i CrossVector3i(SVector3i vec1, SVector3i vec2);
SVector3f CrossVector3f(SVector3f vec1, SVector3f vec2);

/* calculate normal */
SVector3i CalcNormalVector3i(SVector3i vert1, SVector3i vert2, SVector3i vert3);
SVector3f CalcNormalVector3f(SVector3f vert1, SVector3f vert2, SVector3f vert3);

/* Quake 2 manipulations */
typedef float SQ2Vector3f[3];
#define Q2VectorCopy(a, b) (b[0] = a[0], b[1] = a[1], b[2] = a[2])
#define Q2VectorNegate(a, b) (b[0] = -a[0], b[1] = -a[1], b[2] = -a[2])
#define Q2VectorClear(a) (a[0] = a[1] = a[2] = 0)
SVector3f Vector3fToQ2Vector3f(const SVector3f vec);    // change axis to Q2-style
SVector3f Q2Vector3fToVector3f(const SVector3f vec);    // change axis from Q2-style
