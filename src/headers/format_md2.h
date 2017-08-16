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

/*
  Thanks to :
    David Henry (http://tfc.duke.free.fr/coding/md2-specs-en.html)
*/


#pragma once

#include "types.h"
#include "vector.h"

// These are the needed defines for the max values when loading .MD2 files
#define MD2_MAX_TRIANGLES   4096
#define MD2_MAX_VERTICES	2048
#define MD2_MAX_TEXCOORDS	2048
#define MD2_MAX_FRAMES		512
#define MD2_MAX_SKINS		32
#define MD2_MAX_FRAME_NAME  16
#define MD2_MAX_FRAME_SIZE	(MD2_MAX_VERTICES * 4 + 128)

// This holds the header information that is read in at the beginning of the file
/* MD2 header */
typedef struct
{
    int32 ident;              /* magic number: "IDP2" */
    int32 version;            /* version: must be 8 */

    int32 skinWidth;          /* texture width */
    int32 skinHeight;         /* texture height */

    int32 frameSize;          /* size in bytes of a frame */

    int32 numOfSkins;         /* number of skins */
    int32 numOfVertices;      /* number of vertices per frame */
    int32 numOfSt;            /* number of texture coordinates */
    int32 numOfTris;          /* number of triangles */
    int32 numOfGlCmds;        /* number of opengl commands */
    int32 numOfFrames;        /* number of frames */

    int32 offsetSkins;        /* offset skin data */
    int32 offsetSt;           /* offset texture coordinate data */
    int32 offsetTris;         /* offset triangle data */
    int32 offsetFrames;       /* offset frame data */
    int32 offsetGlCmds;       /* offset OpenGL command data */
    int32 offsetEnd;          /* offset end of file */
} SHeaderMd2;

// текстурное им€
/* texture file name */
typedef char TSkinMd2[64];

// текстурные координаты
/* Texture coords */
/* Texture coordinates are stored in a structure as short integers. To get the true texture
   coordinates, you have to divide s by skinwidth and t by skinheight */
typedef struct
{
    int16   s, t;
} STexCoordMd2;

// полигоны
/* Triangle info */
/* Each triangle has an array of vertex indices and an array of texture coordinate indices. */
typedef struct
{
    uint16  vertex[3];   /* vertex indices of the triangle */
    uint16  st[3];       /* tex. coord. indices */
} STriangleMd2;

// вершины представлены в виде "сжатых" 3D-координат + ссылка на "предрасчитанные" нормали
/* Compressed vertex */
/* Vertices are composed of УcompressedФ 3D coordinates, which are stored in one byte for
   each coordinate, and of a normal vector index. The normal vector array is stored in the
   anorms.h file of Quake 2 and hold 162 vectors in floating point (3 float). */
typedef struct
{
    uint8   v[3];         /* position */
    uint8   normalIndex;  /* normal vector index */
} SVertexMd2;

// кадр
/* Model frame */
/* Frames have specific informations for itself and the vertex list of the frame. Informations
   are used to uncompress vertices and obtain the real coordinates. */
typedef struct
{
    SVector3f   scale;                      /* scale factor */
    SVector3f   translate;                  /* translation vector */
    char        name[MD2_MAX_FRAME_NAME];   /* frame name */
    SVertexMd2* verts;                      /* list of frame's vertices */
} SFrameMd2;

// To uncompress vertex coordinates, you need to multiply each
// component by the scale factor and then add the respective
// translation component:
// vec3_t v;                     /* real vertex coords. */
// struct md2_vertex_t vtx;      /* compressed vertex */
// struct md2_frame_t frame;     /* a model frame */
//
// v[i] = (vtx.v[i] * frame.scale[i]) + frame.translate[i];

// GL-команды
// OpenGL commands are structured data for drawing the model using only GL_TRIANGLE_FAN and GL_TRIANGLE_STRIP primitives. It's an array of integers (int) which can be read in packets:
// ХThe first integer is the number of vertices to draw for a new primitive. If it's a positive value, the primitive is GL_TRIANGLE_STRIP, otherwise it's a GL_TRIANGLE_FAN.
// ХThe next integers can be taken by packet of 3 for as many vertices as there is to draw. The two first are the texture coordinates in floating point and the third is the vertex index to draw.
// ХWhen the number of vertices to draw is 0, then we have finished rendering the model.
// GL command packet
typedef struct
{
    float   s;          // s texture coord.
    float   t;          // t texture coord.
    int32   index;      // vertex index
} SGLcmdPacketMd2;

// команда
typedef struct
{
    int32   cmd;
    SGLcmdPacketMd2* packets;
} SGLcmdMd2;

// вс€ информаци€ о MD2-модели
typedef struct
{
    SHeaderMd2    header;      // The header data
    TSkinMd2*     skins;       // The skin data
    STriangleMd2* triangles;   // Face index information
    SFrameMd2*    frames;      // The frames of animation (vertices)
    SVector2f*    texCoords;   // texcoords for 1 frame
                               // текстурные координаты вообще не измен€ютс€ из кадра в кадр и
                               // можно сделать их "предрасчет"
} SModelMd2;
