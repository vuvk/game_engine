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

/** ERROR CODES */
#define ERROR_ENGINE_NOT_CREATED     1
#define ERROR_ENGINE_NOT_STARTED     2
#define ERROR_WINDOW_NOT_CREATED     3
#define ERROR_RENDERER_NOT_CREATED   4
#define ERROR_GL_CONTEXT_NOT_CREATED 5

/** maximums */
#define MAX_NAME_LENGTH    64
#define MAX_PATH_LENGTH    256
#define MAX_STRING_LENGTH  1024

/** ENABLE TESTS */
#ifdef _DEBUG
    #define _TEXTURE_DEBUG
    #define _MATERIAL_DEBUG

    #define _IMAGE_DEBUG
    #define _FONT_DEBUG
    #define _TEXT_DEBUG
#endif // DEBUG
