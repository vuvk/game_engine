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

#include "defines.h"
#include "list.h"
#include "types.h"
#include "mesh_md2.h"


typedef void* aNodeMd2;

/** create an empty node2d */
aNodeMd2 NodeMd2Create(const char* name);
/** destroy node2d */
void NodeMd2Destroy(aNodeMd2* nodeMd2);
/** clear all links to used meshes */
bool NodeMd2ClearAllLinks(aNodeMd2 nodeMd2);

/* manipulation with resources */
/** add link to md2-mesh for using in node. return true if all's OK */
bool NodeMd2AddLinkToMeshMd2(aNodeMd2 nodeMd2, aMeshMd2 meshMd2);
/** remove link to md2-mesh from node */
bool NodeMd2DeleteLinkToMeshMd2(aNodeMd2 nodeMd2, aMeshMd2 meshMd2);
/** remove link to md2-mesh from node by number */
bool NodeMd2DeleteLinkToMeshMd2ByNumber(aNodeMd2 nodeMd2, uint32 numOfMesh);


/* GETTERS */
/** get name of node */
const char* NodeMd2GetName(aNodeMd2 nodeMd2);
/** get all pointers to results by name */
void NodesMd2GetByName(const char* name, SList* results);
/** get visibility of node */
bool NodeMd2IsVisible(aNodeMd2 nodeMd2);

/* get transforms */
/** get pivot point of node */
bool NodeMd2GetPivotf(aNodeMd2 nodeMd2, float* x, float* y, float* z);
SVector3f NodeMd2GetPivotv(aNodeMd2 nodeMd2);
float NodeMd2GetPivotX(aNodeMd2 nodeMd2);
float NodeMd2GetPivotY(aNodeMd2 nodeMd2);
float NodeMd2GetPivotZ(aNodeMd2 nodeMd2);
/** get position of node */
bool NodeMd2GetPosf(aNodeMd2 nodeMd2, float* x, float* y, float* z);
SVector3f NodeMd2GetPosv(aNodeMd2 nodeMd2);
float NodeMd2GetPosX(aNodeMd2 nodeMd2);
float NodeMd2GetPosY(aNodeMd2 nodeMd2);
float NodeMd2GetPosZ(aNodeMd2 nodeMd2);
/** get scale of node */
bool NodeMd2GetScalef(aNodeMd2 nodeMd2, float* x, float* y, float* z);
SVector3f NodeMd2GetScalev(aNodeMd2 nodeMd2);
float NodeMd2GetScaleX(aNodeMd2 nodeMd2);
float NodeMd2GetScaleY(aNodeMd2 nodeMd2);
float NodeMd2GetScaleZ(aNodeMd2 nodeMd2);
/** get rotation of node */
bool NodeMd2GetRotation(aNodeMd2 nodeMd2, float* pitch, float* yaw, float* roll);
float NodeMd2GetPitch(aNodeMd2 nodeMd2);
float NodeMd2GetYaw(aNodeMd2 nodeMd2);
float NodeMd2GetRoll(aNodeMd2 nodeMd2);

/** get first mesh used in node */
#define NodeMd2GetMeshMd2(nodeMd2) NodeMd2GetMeshMd2Ex(nodeMd2, 0)
/** get mesh by number used in node */
aMeshMd2 NodeMd2GetMeshMd2Ex(aNodeMd2 nodeMd2, uint32 numOfMesh);
/** get first material used in node */
#define NodeMd2GetMaterial(nodeMd2) NodeMd2GetMaterialEx(nodeMd2, 0)
/** get material by number used in node */
aMaterial NodeMd2GetMaterialEx(aNodeMd2 nodeMd2, uint32 numOfMaterial);
/** get first speed of animation in node */
#define NodeMd2GetAnimSpeed(nodeMd2) NodeMd2GetAnimSpeedEx(nodeMd2, 0)
/** get speed of animation in node */
float NodeMd2GetAnimSpeedEx(aNodeMd2 nodeMd2, uint32 numOfMesh);
/** get number of current frame in animation */
#define NodeMd2GetCurrentFrame(nodeMd2) NodeMd2GetCurrentFrameEx(nodeMd2, 0)
int32 NodeMd2GetCurrentFrameEx(aNodeMd2 nodeMd2, uint32 numOfMesh);
/** get start and end frame of animation in node */
#define NodeMd2GetInterval(nodeMd2, startFrame, endFrame) NodeMd2GetIntervalEx(nodeMd2, startFrame, endFrame, 0)
bool NodeMd2GetIntervalEx(aNodeMd2 nodeMd2, int32* startFrame, int32* endFrame, uint32 numOfMesh);
/** get looping of animation (by number of linked mesh)*/
#define NodeMd2GetAnimLoop(nodeMd2) NodeMd2GetAnimLoopEx(nodeMd2, 0)
bool NodeMd2GetAnimLoopEx(aNodeMd2 nodeMd2, uint32 numOfMesh);
/** get count of links */
int32 NodeMd2GetCountOfLinks(aNodeMd2 nodeMd2);


/** return true if pointer is nodeMd2 */
bool ObjectIsNodeMd2(void* object);
/* exit from function if pointer is invalid */
#define IS_NODEMD2_VALID(node)        \
        if (!ObjectIsNodeMd2(node))   \
            return 0;


/* SETTERS */
/** set name of node */
bool NodeMd2SetName(aNodeMd2 nodeMd2, const char* name);

/* set transforms */
/** set pivot point of node */
bool NodeMd2SetPivotf(aNodeMd2 nodeMd2, float x, float y, float z);
bool NodeMd2SetPivotv(aNodeMd2 nodeMd2, SVector3f pivot);
bool NodeMd2SetPivotX(aNodeMd2 nodeMd2, float x);
bool NodeMd2SetPivotY(aNodeMd2 nodeMd2, float y);
bool NodeMd2SetPivotZ(aNodeMd2 nodeMd2, float z);
/** set position of node */
bool NodeMd2SetPosf(aNodeMd2 nodeMd2, float x, float y, float z);
bool NodeMd2SetPosv(aNodeMd2 nodeMd2, SVector3f position);
bool NodeMd2SetPosX(aNodeMd2 nodeMd2, float x);
bool NodeMd2SetPosY(aNodeMd2 nodeMd2, float y);
bool NodeMd2SetPosZ(aNodeMd2 nodeMd2, float z);
/** set scale of node */
bool NodeMd2SetScalef(aNodeMd2 nodeMd2, float x, float y, float z);
bool NodeMd2SetScalev(aNodeMd2 nodeMd2, SVector3f scale);
bool NodeMd2SetScaleX(aNodeMd2 nodeMd2, float x);
bool NodeMd2SetScaleY(aNodeMd2 nodeMd2, float y);
bool NodeMd2SetScaleZ(aNodeMd2 nodeMd2, float z);
/** set rotation of node. Angle in degrees */
bool NodeMd2SetRotationInDeg(aNodeMd2 nodeMd2, float pitch, float yaw, float roll);
bool NodeMd2SetPitchInDeg(aNodeMd2 nodeMd2, float pitch);
bool NodeMd2SetYawInDeg(aNodeMd2 nodeMd2, float yaw);
bool NodeMd2SetRollInDeg(aNodeMd2 nodeMd2, float roll);
/** set rotation of node. Angle in radians */
bool NodeMd2SetRotationInRad(aNodeMd2 nodeMd2, float pitch, float yaw, float roll);
bool NodeMd2SetPitchInRad(aNodeMd2 nodeMd2, float pitch);
bool NodeMd2SetYawInRad(aNodeMd2 nodeMd2, float yaw);
bool NodeMd2SetRollInRad(aNodeMd2 nodeMd2, float roll);

/** set first mesh used in node */
#define NodeMd2SetMeshMd2(nodeMd2, meshMd2) NodeMd2SetMeshMd2Ex(nodeMd2, meshMd2, 0)
/** set mesh by number used in node */
bool NodeMd2SetMeshMd2Ex(aNodeMd2 nodeMd2, aMeshMd2 meshMd2, uint32 numOfMesh);
/** set first material used in node */
#define NodeMd2SetMaterial(nodeMd2, material) NodeMd2SetMaterialEx(nodeMd2, material, 0)
/** set material by number used in node */
bool NodeMd2SetMaterialEx(aNodeMd2 nodeMd2, aMaterial material, uint32 numOfMaterial);
/** set first speed of animation in node */
#define NodeMd2SetAnimSpeed(nodeMd2, animSpeed) NodeMd2SetAnimSpeedEx(nodeMd2, animSpeed, 0)
/** set speed of animation in node */
bool NodeMd2SetAnimSpeedEx(aNodeMd2 nodeMd2, float animSpeed, uint32 numOfMesh);
/** set number of current frame in animation */
#define NodeMd2SetCurrentFrame(nodeMd2, numOfFrame) NodeMd2SetCurrentFrameEx(nodeMd2, numOfFrame, 0)
bool NodeMd2SetCurrentFrameEx(aNodeMd2 nodeMd2, int32 numOfFrame, uint32 numOfMesh);
/** set start and end frame of animation in node */
#define NodeMd2SetInterval(nodeMd2, startFrame, endFrame) NodeMd2SetIntervalEx(nodeMd2, startFrame, endFrame, 0)
bool NodeMd2SetIntervalEx(aNodeMd2 nodeMd2, int32 startFrame, int32 endFrame, uint32 numOfMesh);
/** set start and end frame of animation by name in node */
#define NodeMd2SetIntervalByName(nodeMd2, animName) NodeMd2SetIntervalByNameEx(nodeMd2, animName, 0)
/** set start and end frame of animation by name in node (by number of linked mesh)*/
bool NodeMd2SetIntervalByNameEx(aNodeMd2 nodeMd2, const char* animName, uint32 numOfMesh);
/** set looping of animation (by number of linked mesh)*/
#define NodeMd2SetAnimLoop(nodeMd2, loop) NodeMd2SetAnimLoopEx(nodeMd2, loop, 0)
bool NodeMd2SetAnimLoopEx(aNodeMd2 nodeMd2, bool loop, uint32 numOfMesh);

/** get count of loaded nodes */
uint32 NodesMd2GetCount();
/**  clear nodes list */
void NodesMd2DestroyAll();

/** animate and draw all meshes in node */
void NodeMd2Draw(aNodeMd2 nodeMd2);
