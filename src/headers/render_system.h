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
#include "matrix.h"

// for GL-extensions may use GLEW or GLEXT
#define _USE_GLEW
#ifdef _USE_GLEW
    #ifndef GLEW_STATIC
    #define GLEW_STATIC 1
    #endif //GLEW_STATIC*/

    #include "GL/glew.h"
    #include <GL/gl.h>
#else
    #ifndef GL_GLEXT_PROTOTYPES
    #define GL_GLEXT_PROTOTYPES 1
    #endif //GL_GLEXT_PROTOTYPES

    #include "SDL2/SDL_opengl.h"
    #include "SDL2/SDL_opengl_glext.h"
#endif // _USE_GLEW

#include <GL/glu.h>


TMatrix4x4 _world;      // world matrix;
TMatrix4x4 _pers;       // perspective matrix;
                        // change in window_system

typedef enum
{
    rsNotCreated,
    rsCreated
} ERenderState;

/** renderer "constructor" */
int32 RenderCreate();
/** renderer "destructor" */
void RenderDestroy();

/** get max native screen resolution to vector2i */
SVector2i RenderGetMaxScreenResolutionv();
/** get max native screen resolution to int32 */
void RenderGetMaxScreenResolutioni(int32* width, int32* height);
/** get max native screen resolution width */
int32 RenderGetMaxScreenResolutionWidth();
/** get max native screen resolution height */
int32 RenderGetMaxScreenResolutionHeight();



//system
ERenderState RenderGetState();
const char* RenderGetVendor();
const char* RenderGetOpenGLVersion();
const char* RenderGetShaderVersion();
int32 RenderGetDepthBufferSize();
int32 RenderGetStencilBufferSize();
/** print info about graphical system */
void RenderPrintInfo();
/** print all available screen modes */
void RenderPrintDisplayModes();
