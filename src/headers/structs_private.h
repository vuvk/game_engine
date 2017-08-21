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
#include "format_bsp.h"

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

    uint32    charsVBO[256]; // индексы VBO дл€ каждой отдельной буквы

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
// MD2-меш с трансформаци€ми и информацией о модели
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

    bool  loop;           // looping of animation
    bool  visible;        // if not visible then not draw

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


/* Mesh bsp */
// bsp-меш с трансформаци€ми и информацией о модели
typedef struct
{
    struct  SHeaderOfClass;
    struct  STransform3D;

    char   fileName[MAX_PATH_LENGTH];

    uint32  numOfVerts;       // The number of verts in the model
    uint32  numOfEdges;
    uint32  numOfFaces;       // The number of faces in the model
    uint32  numOfLeaves;
    uint32  numOfNodes;
    uint32  numOfPlanes;
    uint32  numOfTexInfo;
    //uint32  numOfTextures;    // The number of texture maps
    //uint32  numOfIndices;     // The number of indices for the model
    //uint32  numOfLightmaps;   // The number of light maps

    //int32*      indices;     // The object's indices for rendering
    //SVertexBsp* vertices;    // The object's vertices
    //SFaceBsp*   faces;       // The faces information of the object

    SVertexBsp*  vertices;
    SEdgeBsp*    edges;
    SFaceBsp*    faces;
    SLeafBsp*    leaves;
    SNodeMapBsp* nodes;     // nodes for BSP tree
    SPlaneBsp*   planes;    // Planes for BSP tree
    STexInfoBsp* texInfo;

    uint8*  visInfo;  // visibility data
    int32* faceEdgeTable;
    int16* leafFaceTable;

    /*
    // поверхности
    faceDirectory : array of TBSPFaceDirectoryEntry;  // массив, хран€щий ссылки по типу
                                                      // поверхности на массив поверхностей
    // обычные полигоны
    numOfPolygonFaces : GLint;
    polygonFaces : array of TBSPPolygonFace;

    // меши (модели MD3)
    numOfMeshFaces : GLint;
    meshFaces : array of TBSPMeshFace;

    // патчи
    numOfPatches : GLint;
    patches : array of TBSPBezierPatch;

    // список ентитей
    entityString : PChar;

    // Leaves
    numOfLeaves : GLint;
    leaves : array of TBSPLeaf;

    // leaf faces array
    leafFaces : array of GLint;

    numOfPlanes : GLint;
    //planes : array of TPlane;
    planes : array of TBSPPlane;

    numOfNodes : GLint;
    nodes : array of TBSPNode;


    visData : TBSPVisData;
    */
} SMeshBsp;


//-----------------------
//        CAMERA
//-----------------------

typedef struct
{
    struct    SHeaderOfClass;

    char*     name;

    SVector3f position;  // ѕозици€ камеры
    SVector3f target;    // Ќаправление камеры
    SVector3f upVector;  // ¬ертикальный вектор
} SCamera;


/* for scene_manager. Now is 3d or 2d mode? */
bool _is3dMode;

//#pragma pack(pop)
