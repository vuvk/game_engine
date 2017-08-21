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
    Max McGuire (http://www.flipcode.com/archives/Quake_2_BSP_File_Format.shtml)
    Q2DP        (http://www.gamers.org/dEngine/quake2/Q2DP/Q2DP_Map/Q2DP_Map.shtml)
    jayschwa    (https://github.com/jayschwa/Quake2.jl)
*/

#pragma once

#include "types.h"
#include "vector.h"

typedef SVector3f SVertexBsp;

typedef struct
{
    uint32  offset;     // offset (in bytes) of the data from the beginning of the file
    uint32  length;     // length (in bytes) of the data
} SLumpBsp;

typedef struct
{
    uint32    ident;      // magic number ("IBSP" = 1347633737 = 0x50534249)
    uint32    version;    // version of the BSP format (38)
    SLumpBsp  lump[19];   // directory of the lumps
} SHeaderBsp;

/*
 BSP lumps Index
*/
#define BSP_ENTITIES         0      // MAP entity text buffer
#define BSP_PLANES           1      // Plane array
#define BSP_VERTICES         2      // Vertex array
#define BSP_VISIBILITY       3      // Compressed PVS data and directory for all clusters
#define BSP_NODES            4      // Internal node array for the BSP tree
#define BSP_TEXINFO          5      // Face texture application array
#define BSP_FACES            6      // Face array
#define BSP_LIGHTMAPS        7      // Lightmaps
#define BSP_LEAVES           8      // Internal leaf array of the BSP tree
#define BSP_LEAF_FACE_TABLE  9      // Index lookup table for referencing the face array from a leaf
#define BSP_LEAF_BRUSH_TABLE 10
#define BSP_EDGES            11     // Edge array
#define BSP_FACE_EDGE_TABLE  12     // Index lookup table for referencing the edge array from a face
#define BSP_MODELS           13
#define BSP_BRUSHES          14
#define BSP_BRUSH_SIDES      15
#define BSP_POP              16
#define BSP_AREAS            17
#define BSP_AREA_PORTALS     18

/*
    Not only are vertices shared between faces, but edges are as well. Each edge is stored as a pair of
    indices into the vertex array. The storage is two 16-bit integers, so the number of edges in the edge
    array is the size of the edge lump divided by 4. There is a little complexity here because an edge
    could be shared by two faces with different windings, and therefore there is no particular "direction"
    for an edge. This is further discussed in the section on face edges.
*/
typedef struct
{
    uint16  v1;
    uint16  v2;
} SEdgeBsp;

typedef struct
{
    uint16  plane;           // index of the plane the face is parallel to
    uint16  planeSide;       // set if the normal is parallel to the plane normal

    uint32  firstEdge;       // index of the first edge (in the face edge array)
    uint16  numOfEdges;      // number of consecutive edges (in the face edge array)

    uint16  textureInfo;     // index of the texture info structure

    uint8   lmapStyles[4];   // styles (bit flags) for the lightmaps
    uint32  lmapOffset;      // offset of the lightmap (in bytes) in the lightmap lump
} SFaceBsp;

/*
    The plane lump stores and array of SPlaneBsp structures which are used as the splitting planes in the BSP.

    Each SPlaneBsp structure is 20 bytes, so the number of planes is the size of the plane lump divided by 20.
    The x, y and z components of the normal correspond to A, B, C constants and the distance to the D constant in the plane equation:
        F(x, y, z) = Ax + By + Cz - D
    A point is on the plane is F(x, y, z) = 0, in front of the plane if F(x, y, z) > 0 and behind the plane if F(x, y, z) < 0.
    This is used in the traversal of the BSP tree is traversed.
*/
typedef struct
{
    SVector3f  normal;    // A, B, C components of the plane equation
    float    distance;    // D component of the plane equation
    uint32   type;        // ?
} SPlaneBsp;


/*
    The nodes are stored as an array in the node lump, where the first element is the root of the BSP tree

    As mentioned earlier, the faces listed in the node are not used for rendering but rather for collision detection.
*/
typedef struct
{
    uint32  plane;        // index of the splitting plane (in the plane array)

    int32   frontChild;   // index of the front child node or leaf
    int32   backChild;    // index of the back child node or leaf

    SVector3s  bboxMin;      // minimum x, y and z of the bounding box
    SVector3s  bboxMax;      // maximum x, y and z of the bounding box

    uint16  firstFace;    // index of the first face (in the face array)
    uint16  numOfFaces;   // number of consecutive edges (in the face array)
} SNodeMapBsp;

/*
    The leaf lump stores an array of SLeafBsp structures which are the leaves of the BSP tree

    Leaves are grouped into clusters for the purpose of storing the PVS, and the cluster field
    gives the index into the array stored in the visibility lump. See the Visibility section for
    more information on this. If the cluster is -1, then the leaf has no visibility information
    (in this case the leaf is not a place that is reachable by the player).
*/
typedef struct
{
    uint32  brush_or;          // ?

    uint16  cluster;           // -1 for cluster indicates no visibility information
    uint16  area;              // ?

    SVector3s  bboxMin;           // bounding box minimums
    SVector3s  bboxMax;           // bounding box maximums

    uint16  firstLeafFace;     // index of the first face (in the face leaf array)
    uint16  numOfLeafFaces;    // number of consecutive edges (in the face leaf array)

    uint16  firstLeafBrush;    // ?
    uint16  numOfLeafBrushes;  // ?
} SLeafBsp;

/*
    Texture information structures specify all of the details about how a face is textured.

    u = x * uAxis.x + y * uAxis.y + z * uAxis.z + uOffset
    v = x * uAxis.x + y * uAxis.y + z * uAxis.z + uOffset
*/
typedef struct
{
    SVector3f uAxis;
    float     uOffset;

    SVector3f vAxis;
    float     vOffset;

    uint32  flags;
    uint32  value;

    char    textureName[32];

    uint32  nextTexInfo;
} STexInfoBsp;


/*
  int v = offset;

  memset(clusterVisible, 0, numOfClusters);

  for (int c = 0; c < numOfClusters; ++v)
  {
    if (pvsBuffer[v] == 0)
    {
      ++v;
      c += 8 * pvsBuffer[v];
    }
    else
    {
      for (uint8 bit = 1; bit != 0; bit *= 2, ++c)
      {
        if (pvsBuffer[v] & bit)
        {
            clusterVisible[c] = 1;
        }
      }
    }
  }
*/
typedef struct
{
	uint32 pvs;   // offset (in bytes) from the beginning of the visibility lump
	uint32 phs;   // ?
} SVisOffsetBsp;


typedef struct
{
    char    name[32];        // name of the texture

    uint32  width;           // width (in pixels) of the largest mipmap level
    uint32  height;          // height (in pixels) of the largest mipmap level

    int32   offset[4];       // byte offset of the start of each of the 4 mipmap levels

    char    nextName[32];    // name of the next texture in the animation

    uint32  flags;           // ?
    uint32  contents;        // ?
    uint32  value;           // ?
} SHeaderWal;
