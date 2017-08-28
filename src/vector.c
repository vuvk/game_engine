//#include <math.h>

#include "SDL2/SDL.h"


#include "vector.h"

/* "constructors" */
inline SVector2i NewVector2i(int32 x, int32 y)
{
	SVector2i v = {x, y};
	return v;
}

inline SVector2f NewVector2f(float x, float y)
{
	SVector2f v = {x, y};
	return v;
}

inline SVector3s NewVector3s(int16 x, int16 y, int16 z)
{
	SVector3s v = {x, y, z};
	return v;
}

inline SVector3i NewVector3i(int32 x, int32 y, int32 z)
{
	SVector3i v = {x, y, z};
	return v;
}

inline SVector3f NewVector3f(float x, float y, float z)
{
	SVector3f v = {x, y, z};
	return v;
}

inline SVector4i NewVector4i(int32 x, int32 y, int32 z, int32 w)
{
	SVector4i v = {x, y, z, w};
	return v;
}

inline SVector4f NewVector4f(float x, float y, float z, float w)
{
	SVector4f v = {x, y, z, w};
	return v;
}

/* transformation from .. to .. */
inline SVector3f Vector2fToVector3f(SVector2f vec)
{
    SVector3f v = {vec.x, vec.y, 0.0f};
    return v;
}

inline SVector3f Vector4fToVector3f(SVector4f vec)
{
    SVector3f v = { vec.x / vec.w,
                    vec.y / vec.w,
                    vec.z / vec.w };
    return v;
}

inline SVector4f Vector3fToVector4f(SVector3f vec)
{
    SVector4f v = {vec.x, vec.y, vec.z, 1.0f};
    return v;
}

/* equals */
inline bool IsEqualVector2i(SVector2i vec1, SVector2i vec2)
{
	return (vec1.x == vec2.x &&
            vec1.y == vec2.y);
}

inline bool IsEqualVector2f(SVector2f vec1, SVector2f vec2)
{
	return (vec1.x == vec2.x &&
            vec1.y == vec2.y);
}

inline bool IsEqualVector3s(SVector3s vec1, SVector3s vec2)
{
	return (vec1.x == vec2.x &&
            vec1.y == vec2.y &&
            vec1.z == vec2.z);
}

inline bool IsEqualVector3i(SVector3i vec1, SVector3i vec2)
{
	return (vec1.x == vec2.x &&
            vec1.y == vec2.y &&
            vec1.z == vec2.z);
}

inline bool IsEqualVector3f(SVector3f vec1, SVector3f vec2)
{
	return(vec1.x == vec2.x &&
            vec1.y == vec2.y &&
            vec1.z == vec2.z);
}

inline bool IsEqualVector4i(SVector4i vec1, SVector4i vec2)
{
	return (vec1.x == vec2.x &&
            vec1.y == vec2.y &&
            vec1.z == vec2.z &&
            vec1.w == vec2.w);
}

inline bool IsEqualVector4f(SVector4f vec1, SVector4f vec2)
{
	return (vec1.x == vec2.x &&
            vec1.y == vec2.y &&
            vec1.z == vec2.z &&
            vec1.w == vec2.w);
}

/*******************/
/* math operations */
/*******************/
/* addition */
inline SVector2i AddVector2i(SVector2i vec1, SVector2i vec2)
{
	SVector2i v = { vec1.x + vec2.x,
                    vec1.y + vec2.y };
	return v;
}

inline SVector2f AddVector2f(SVector2f vec1, SVector2f vec2)
{
	SVector2f v = { vec1.x + vec2.x,
                    vec1.y + vec2.y };
	return v;
}

inline SVector3s AddVector3s(SVector3s vec1, SVector3s vec2)
{
	SVector3s v = { vec1.x + vec2.x,
                    vec1.y + vec2.y,
                    vec1.z + vec2.z };
	return v;
}

inline SVector3i AddVector3i(SVector3i vec1, SVector3i vec2)
{
	SVector3i v = { vec1.x + vec2.x,
                    vec1.y + vec2.y,
                    vec1.z + vec2.z };
	return v;
}

inline SVector3f AddVector3f(SVector3f vec1, SVector3f vec2)
{
	SVector3f v = { vec1.x + vec2.x,
                    vec1.y + vec2.y,
                    vec1.z + vec2.z };
	return v;
}

inline SVector4i AddVector4i(SVector4i vec1, SVector4i vec2)
{
	SVector4i v = { vec1.x + vec2.x,
                    vec1.y + vec2.y,
                    vec1.z + vec2.z,
                    vec1.w + vec2.w };
	return v;
}

inline SVector4f AddVector4f(SVector4f vec1, SVector4f vec2)
{
	SVector4f v = { vec1.x + vec2.x,
                    vec1.y + vec2.y,
                    vec1.z + vec2.z,
                    vec1.w + vec2.w };
	return v;
}

/* subtract */
inline SVector2i SubVector2i(SVector2i vec1, SVector2i vec2)
{
	SVector2i v = { vec1.x - vec2.x,
                    vec1.y - vec2.y };
	return v;
}

inline SVector2f SubVector2f(SVector2f vec1, SVector2f vec2)
{
	SVector2f v = { vec1.x - vec2.x,
                    vec1.y - vec2.y };
	return v;
}

inline SVector3s SubVector3s(SVector3s vec1, SVector3s vec2)
{
	SVector3s v = { vec1.x - vec2.x,
                    vec1.y - vec2.y,
                    vec1.z - vec2.z };
	return v;
}

inline SVector3i SubVector3i(SVector3i vec1, SVector3i vec2)
{
	SVector3i v = { vec1.x - vec2.x,
                    vec1.y - vec2.y,
                    vec1.z - vec2.z };
	return v;
}

inline SVector3f SubVector3f(SVector3f vec1, SVector3f vec2)
{
	SVector3f v = { vec1.x - vec2.x,
                    vec1.y - vec2.y,
                    vec1.z - vec2.z };
	return v;
}

inline SVector4i SubVector4i(SVector4i vec1, SVector4i vec2)
{
	SVector4i v = { vec1.x - vec2.x,
                    vec1.y - vec2.y,
                    vec1.z - vec2.z,
                    vec1.w - vec2.w };
	return v;
}

inline SVector4f SubVector4f(SVector4f vec1, SVector4f vec2)
{
	SVector4f v = { vec1.x - vec2.x,
                    vec1.y - vec2.y,
                    vec1.z - vec2.z,
                    vec1.w - vec2.w  };
	return v;
}

/* multiplication by scalar */
inline SVector2i MultVector2i(SVector2i vec, float scalar)
{
	SVector2i v = { vec.x *(int32)scalar,
                    vec.y *(int32)scalar };
	return v;
}

inline SVector2f MultVector2f(SVector2f vec, float scalar)
{
	SVector2f v = { vec.x * scalar,
                    vec.y * scalar };
	return v;
}

inline SVector3s MultVector3s(SVector3s vec, float scalar)
{
	SVector3s v = { vec.x *(int16)scalar,
                    vec.y *(int16)scalar,
                    vec.z *(int16)scalar };
	return v;
}

inline SVector3i MultVector3i(SVector3i vec, float scalar)
{
	SVector3i v = { vec.x *(int32)scalar,
                    vec.y *(int32)scalar,
                    vec.z *(int32)scalar };
	return v;
}

inline SVector3f MultVector3f(SVector3f vec, float scalar)
{
	SVector3f v = { vec.x * scalar,
                    vec.y * scalar,
                    vec.z * scalar };
	return v;
}

inline SVector4i MultVector4i(SVector4i vec, float scalar)
{
	SVector4i v = { vec.x *(int32)scalar,
                    vec.y *(int32)scalar,
                    vec.z *(int32)scalar,
                    vec.w *(int32)scalar };
	return v;
}

inline SVector4f MultVector4f(SVector4f vec, float scalar)
{
	SVector4f v = { vec.x * scalar,
                    vec.y * scalar,
                    vec.z * scalar,
                    vec.w * scalar };
	return v;
}

/* division by scalar */
inline SVector2i DivVector2i(SVector2i vec, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    else
        return vZero2i;

	return MultVector2i(vec, scalar);
}

inline SVector2f DivVector2f(SVector2f vec, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    else
        return vZero2f;

	return MultVector2f(vec, scalar);
}

inline SVector3s DivVector3s(SVector3s vec, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    else
        return vZero3s;

	return MultVector3s(vec, scalar);
}

inline SVector3i DivVector3i(SVector3i vec, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    else
        return vZero3i;

	return MultVector3i(vec, scalar);
}

inline SVector3f DivVector3f(SVector3f vec, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    else
        return vZero3f;

	return MultVector3f(vec, scalar);
}

inline SVector4i DivVector4i(SVector4i vec, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    else
        return vZero4i;

	return MultVector4i(vec, scalar);
}

inline SVector4f DivVector4f(SVector4f vec, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    else
        return vZero4f;

	return MultVector4f(vec, scalar);
}

/* magnitude of vector */
inline float LenVector2i(SVector2i vec)
{
	return SDL_sqrt(vec.x*vec.x + vec.y*vec.y);
}

inline float LenVector2f(SVector2f vec)
{
	return SDL_sqrt(vec.x*vec.x + vec.y*vec.y);
}

inline float LenVector3s(SVector3s vec)
{
	return SDL_sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

inline float LenVector3i(SVector3i vec)
{
	return SDL_sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

inline float LenVector3f(SVector3f vec)
{
	return SDL_sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

/* normalize vector */
inline SVector2i NormalizeVector2i(SVector2i vec)
{
	float len = LenVector2i(vec);
	return DivVector2i(vec, len);
}

inline SVector2f NormalizeVector2f(SVector2f vec)
{
	float len = LenVector2f(vec);
	return DivVector2f(vec, len);
}

inline SVector3s NormalizeVector3s(SVector3s vec)
{
	float len = LenVector3s(vec);
	return DivVector3s(vec, len);
}

inline SVector3i NormalizeVector3i(SVector3i vec)
{
	float len = LenVector3i(vec);
	return DivVector3i(vec, len);
}

inline SVector3f NormalizeVector3f(SVector3f vec)
{
	float len = LenVector3f(vec);
	return DivVector3f(vec, len);
}

/* dot product */
inline float DotVector2i(SVector2i vec1, SVector2i vec2)
{
	return vec1.x*vec2.x + vec1.y*vec2.y;
}

inline float DotVector2f(SVector2f vec1, SVector2f vec2)
{
	return vec1.x*vec2.x + vec1.y*vec2.y;
}

inline float DotVector3s(SVector3s vec1, SVector3s vec2)
{
	return vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
}

inline float DotVector3i(SVector3i vec1, SVector3i vec2)
{
	return vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
}

inline float DotVector3f(SVector3f vec1, SVector3f vec2)
{
	return vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
}

/* cross product */
SVector3s CrossVector3s(SVector3s vec1, SVector3s vec2)
{
	SVector3s v;
	v.x = vec1.y*vec2.z - vec1.z*vec2.y;
	v.y = vec1.z*vec2.x - vec1.x*vec2.z;
	v.z = vec1.x*vec2.y - vec1.y*vec2.x;

	return v;
}

SVector3i CrossVector3i(SVector3i vec1, SVector3i vec2)
{
	SVector3i v;
	v.x = vec1.y*vec2.z - vec1.z*vec2.y;
	v.y = vec1.z*vec2.x - vec1.x*vec2.z;
	v.z = vec1.x*vec2.y - vec1.y*vec2.x;

	return v;
}

SVector3f CrossVector3f(SVector3f vec1, SVector3f vec2)
{
	SVector3f v;
	v.x = vec1.y*vec2.z - vec1.z*vec2.y;
	v.y = vec1.z*vec2.x - vec1.x*vec2.z;
	v.z = vec1.x*vec2.y - vec1.y*vec2.x;

	return v;
}

/* calculate normal */
SVector3i CalcNormalVector3i(SVector3i vert1, SVector3i vert2, SVector3i vert3)
{
    int32 Qx, Qy, Qz, Px, Py, Pz;
    SVector3i v;

    Qx = vert2.x - vert1.x;
    Qy = vert2.y - vert1.y;
    Qz = vert2.z - vert1.z;

    Px = vert3.x - vert1.x;
    Py = vert3.y - vert1.y;
    Pz = vert3.z - vert1.z;

    v.x = Py * Qz - Pz * Qy;
    v.y = Pz * Qx - Px * Qz;
    v.z = Px * Qy - Py * Qx;

    return v;
}

SVector3f CalcNormalVector3f(SVector3f vert1, SVector3f vert2, SVector3f vert3)
{
	/* расчет нормали по трем вершинам из книги Краснова */
    float Qx, Qy, Qz, Px, Py, Pz;
    SVector3f v;

    Qx = vert2.x - vert1.x;
    Qy = vert2.y - vert1.y;
    Qz = vert2.z - vert1.z;

    Px = vert3.x - vert1.x;
    Py = vert3.y - vert1.y;
    Pz = vert3.z - vert1.z;

    v.x = Py * Qz - Pz * Qy;
    v.y = Pz * Qx - Px * Qz;
    v.z = Px * Qy - Py * Qx;

    return v;
}


SVector3f Vector3fToQ2Vector3f(const SVector3f vec)
{
    SVector3f v = { vec.x,
                   -vec.z,
                    vec.y };

    return v;
}

SVector3f Q2Vector3fToVector3f(const SVector3f vec)
{
    SVector3f v = { vec.x,
                    vec.z,
                   -vec.y };

    return v;
}
