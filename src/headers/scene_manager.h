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


/** start draw scene */
void SceneBegin();
/** end draw scene */
void SceneEnd();

/** set clear color before draw (without alpha) */
#define SceneSetClearColor(r, g, b) SceneSetClearColorEx(r, g, b, 1.0f);
/** set clear color before draw (with alpha) */
void SceneSetClearColorEx(float r, float g, float b, float a);

/** automatically draw all 3d-nodes */
void SceneDrawNodes();

/** automatically draw all 2d-nodes */
void SceneDrawNodes2D();

/** automatically draw all nodes (2d and 3d) */
void SceneDrawAllNodes();
