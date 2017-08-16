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
    Wingman (http://masandilov.ru/opengl)
*/

#pragma once

#include "types.h"



typedef void* aCamera;

/** create a camera */
aCamera CameraCreate(const char* name);
/** destroy a camera */
void CameraDestroy(aCamera* camera);

// ��� ���������� ���������, ����������� � ����. ������ ������.
bool CameraPlacev (aCamera camera, SVector3f position, SVector3f view, SVector3f upVector);
bool CameraPlacef (aCamera camera,
                   float positionX, float positionY, float positionZ,
                   float viewX,     float viewY,     float viewZ,
                   float upVectorX, float upVectorY, float upVectorZ);
// ��� �-� ������� ������ ������ ������� � ����������� �� ���������� ����������.
bool CameraRotatef(aCamera camera, float angle, float x, float y, float z);
bool CameraRotatev(aCamera camera, float angle, SVector3f axis);
/** move camera to forward (speed > 0) or backward (speed < 0) */
bool CameraMove(aCamera camera, float speed);
/** move camera to right (speed > 0) or left (speed < 0) */
bool CameraStrafe(aCamera camera, float speed);
/** change view direction of camera by nouse */
bool CameraSetViewByMouse(aCamera camera);

/** return true if pointer is camera */
bool ObjectIsCamera(void* object);
/* exit from function if pointer is invalid */
#define IS_CAMERA_VALID(camera)        \
        if (!ObjectIsCamera(camera))   \
            return 0;