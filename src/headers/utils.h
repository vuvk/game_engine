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
#include "color.h"
#include "vector.h"
#include "SDL2/SDL_stdinc.h"


/* math */
float DegToRad(float deg);
float RadToDeg(float rad);
static const float PI_2 = 2.0f*M_PI;

/* limit number in range */
void LimitFloat(float* number, float min, float max);
void LimitShort(int16* number, int16 min, int16 max);
void LimitInt(int32* number, int32 min, int32 max);
void LimitLong(int64* number, int64 min, int64 max);
#define Limit(X, MIN, MAX) _Generic (                      \
                                     (X),                  \
                                     float* : LimitFloat,  \
                                     int16* : LimitShort,  \
                                     int32* : LimitInt  ,  \
                                     int64* : LimitLong    \
                                    )(X, MIN, MAX)

/* swap anything */
void SwapPtr(void** a, void** b);
void SwapShort(int16* a, int16* b);
void SwapInt(int32* a, int32* b);
void SwapLong(int64* a, int64* b);
void SwapFloat(float* a, float* b);
#define Swap(A, B) _Generic ((A),                   \
                             int16*  : SwapShort,   \
                             int32*  : SwapInt  ,   \
                             int64*  : SwapLong ,   \
                             float*  : SwapFloat,   \
                             default : SwapPtr      \
                            )(A, B)

// operator '+'
#define Add(A, B) _Generic ((A),                         \
                            SVector2f  : AddVector2f,    \
                            SVector2i  : AddVector2i,    \
                            SVector3f  : AddVector3f,    \
                            SVector3s  : AddVector3s,    \
                            SVector3i  : AddVector3i,    \
                            SVector4f  : AddVector4f,    \
                            SVector4i  : AddVector4i,    \
                            RGBColor   : AddRGBColor,    \
                            RGBColorf  : AddRGBColorf,   \
                            RGBAColor  : AddRGBAColor,   \
                            RGBAColorf : AddRGBAColorf   \
                           )(A, B)
// operator '-'
#define Sub(A, B) _Generic ((A),                         \
                            SVector2f  : SubVector2f,    \
                            SVector2i  : SubVector2i,    \
                            SVector3f  : SubVector3f,    \
                            SVector3s  : SubVector3s,    \
                            SVector3i  : SubVector3i,    \
                            SVector4f  : SubVector4f,    \
                            SVector4i  : SubVector4i,    \
                            RGBColor   : SubRGBColor,    \
                            RGBColorf  : SubRGBColorf,   \
                            RGBAColor  : SubRGBAColor,   \
                            RGBAColorf : SubRGBAColorf   \
                           )(A, B)
// operator '*'
#define Mult(A, B) _Generic ((A),                           \
                             SVector2f  : MultVector2f,     \
                             SVector2i  : MultVector2i,     \
                             SVector3f  : MultVector3f,     \
                             SVector3s  : MultVector3s,     \
                             SVector3i  : MultVector3i,     \
                             SVector4f  : MultVector4f,     \
                             SVector4i  : MultVector4i,     \
                             RGBColor   : MultRGBColor,     \
                             RGBColorf  : MultRGBColorf,    \
                             RGBAColor  : MultRGBAColor,    \
                             RGBAColorf : MultRGBAColorf    \
                             )(A, B)
// operator '/'
#define Div(A, B) _Generic ((A),                          \
                            SVector2f  : DivVector2f,     \
                            SVector2i  : DivVector2i,     \
                            SVector3f  : DivVector3f,     \
                            SVector3s  : DivVector3s,     \
                            SVector3i  : DivVector3i,     \
                            SVector4f  : DivVector4f,     \
                            SVector4i  : DivVector4i,     \
                            RGBColor   : DivRGBColor,     \
                            RGBColorf  : DivRGBColorf,    \
                            RGBAColor  : DivRGBAColor,    \
                            RGBAColorf : DivRGBAColorf    \
                           )(A, B)

/* strings */
/** Safely compares strings
 *  return:
 *      true  if compares
 *      false if not compares
 *      -1    if error
 */
int8 StrEqual (const char* str1, const char* str2);
/** Safely copy string from src(source) to dst(destination)
 *  limit length string as maxLength WITH null-terminator \0!!
 */
void StrCopy(char* dst, const char* src, uint32 maxLength);
/** Safely calculate length of string WITHOUT null-terminator \0!!
 *  limit length string as MAX_STRING_LENGTH
 *  return -1 if error
 */
int32 StrLength(const char* string);


/* TESTS! */
#ifdef _DEBUG
#include <assert.h>
void UtilsTest();
#endif //_DEBUG
