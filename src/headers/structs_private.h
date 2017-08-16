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

#include <stdio.h>

#include "defines.h"
#include "list.h"
#include "dictionary.h"
#include "types.h"
#include "vector.h"
#include "matrix.h"
#include "texture.h"
#include "material.h"
#include "format_md2.h"

//#pragma pack(push, 1)

/* base of all classes */
struct SHeaderOfClass
{
    uint32 ident;
};


//-----------------------
//  RESOURCES
//-----------------------

/* class of texture */
typedef struct
{
    struct  SHeaderOfClass;

    char    fileName[MAX_PATH_LENGTH];
    uint16  width;
    uint16  height;

    uint32  ogl_id;     // идентификатор текстуры в OpenGL
} STexture;

/* class of material */
typedef struct
{
    struct  SHeaderOfClass;

    int8    cullFace;
    int8    type;
    void*   glProgram;

    aTexture textures[MAX_MATERIAL_LAYERS];
} SMaterial;

/* all textures here */
SDictionary* _textures;
/* all materials here */
SDictionary* _materials;


//-----------------------
//        2D
//-----------------------

/* base of all 2d-transforms */
struct STransform2D
{
    SVector2f   pivot;
    SVector2f   position;
    float       rotation;
    SVector2f   scale;
    TMatrix4x4  transform;
};

/* class of image */
typedef struct
{
    struct     SHeaderOfClass;

    uint16     width;
    uint16     height;
    SVector2f  pivot;

    uint32     indVBO;    // индекс VBO

    STexture*  texture;
    SMaterial* material;
} SImage;

/* class of font */
typedef struct
{
    struct    SHeaderOfClass;
    uint16    charW;
    uint16    charH;

    uint32    charsVBO[256]; // индексы VBO для каждой отдельной буквы

    STexture* texture;
} SFont;

/* class of text */
typedef struct
{
    struct  SHeaderOfClass;

    char*   string;         // = NULL, если текст статический
    uint32  staticVBO;      // 0 - если текст динамический, >0 если сгенерирован VBO со статической надписью
    SFont*  font;

    SVector2f pivot;
} SText;

/* class of 2D-Node */
typedef struct
{
    struct  SHeaderOfClass;
    struct  STransform2D;

    char    name[MAX_NAME_LENGTH];
    SList*  images;
    SList*  texts;
    bool    visible;
} SNode2D;

/* all images here */
SDictionary* _images;
/* all fonts here */
SDictionary* _fonts;
/* all texts here */
SDictionary* _texts;

/* all 2d-nodes here */
SList* _nodes2d;


//-----------------------
//        3D
//-----------------------

/* base of all 3d-transforms */
struct STransform3D
{
    SVector3f   pivot;
    SVector3f   position;
    SVector3f   scale;
    float       pitch, yaw, roll;

    TMatrix4x4  transform;
};

/* Mesh md2 */
// MD2-меш с трансформациями и информацией о модели
typedef struct
{
    struct     SHeaderOfClass;
    struct     STransform3D;

    char       fileName[MAX_PATH_LENGTH];
    SModelMd2* model;
} SMeshMd2;

/* mesh md2 with parameters */
typedef struct
{
    SMeshMd2*  mesh;            // MD2 mesh
    SMaterial* material;
    int32      numOfFramePrev,  // interpolate from...
               numOfFrameNext,  // interpolate to...
               minFrame,        // animate from...
               maxFrame;        // animate to...

    SVector3f* vbuf_prev;     // unpacked vertices for previous frame
    SVector3f* vbuf_next;     // unpacked vertices for next frame

    SVector3f* nbuf_prev;     // unpacked normals for previous frame
    SVector3f* nbuf_next;     // unpacked normals for next frame

    float  animSpeed;
    float  interp;         // value of step (from 0.0f to 1.0f)

    bool   visible;        // if not visible then not draw

} SParamMd2;

/* class of MD2-Node */
typedef struct
{
    struct  SHeaderOfClass;
    struct  STransform3D;

    char    name[MAX_NAME_LENGTH];
    SList*  params;

    bool    visible;       // if not visible then not draw all meshes in node
} SNodeMd2;

/* all md2-meshes here */
SDictionary* _meshesMd2;

/* all md2-nodes here */
SList* _nodesMd2;


//-----------------------
//        CAMERA
//-----------------------

typedef struct
{
    struct    SHeaderOfClass;

    SVector3f position;  // Позиция камеры
    SVector3f view;      // Направление камеры
    SVector3f upVector;  // Вертикальный вектор
} SCamera;


/* for scene_manager. Now is 3d or 2d mode? */
bool _is3dMode;

//#pragma pack(pop)
