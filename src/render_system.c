/*#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>*/
// because it's not in glext.h. Strange
//#define GL_SHADING_LANGUAGE_VERSION 0x8B8C

#include "SDL2/SDL.h"

#include "types.h"
#include "matrix.h"
#include "defines.h"
#include "log_system.h"
#include "render_system.h"
#include "window_system.h"


//static SDL_Renderer* _sdlRenderer;
static SDL_GLContext _sdlGLContext;
static ERenderState _renderState;

int RenderCreate()
{
    /* what do you want if window is not created, hmm??*/
    if(WindowGetState() == wsNotCreated)
    {
        LogWriteError("Could not create OpenGL context, because window not created!\n");
        return ERROR_GL_CONTEXT_NOT_CREATED;
    }

    /* what do you want, if renderer is already created? */
    if(_renderState == rsCreated)
    {
        LogWriteWarning("Renderer is already created!\n");
        return 0;
    }

    /* try to create SDL2 renderer */
    /*_sdlRenderer = SDL_CreateRenderer((SDL_Window*)WindowGetPointer(), -1, SDL_RENDERER_ACCELERATED);
    if(!_sdlRenderer)
    {
        LogWriteError("Could not create renderer ! Error : %s\n", SDL_GetError());
        return ERROR_RENDERER_NOT_CREATED;
    }*/

    /* try to create OpenGL context */
    _sdlGLContext = SDL_GL_CreateContext((SDL_Window*)WindowGetPointer());
    if(!_sdlGLContext)
    {
        LogWriteError("Could not create OpenGL context! Error : %s\n", SDL_GetError());
        return ERROR_GL_CONTEXT_NOT_CREATED;
    }

    // сглаживание
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);

    // тест глубины
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    /* initialize GL extensions */
    #ifdef _USE_GLEW
    glewInit();
    #endif // _USE_GLEW

    /* now render is created */
    _renderState = rsCreated;

    return 0;
}

void RenderDestroy()
{
    SDL_GL_DeleteContext(_sdlGLContext);
    _sdlGLContext = NULL;

    _renderState = rsNotCreated;
}

//system
inline ERenderState RenderGetState()
{
    return _renderState;
}
inline const char* RenderGetVendor()
{
    return (const char*)glGetString(GL_VENDOR);
}
inline const char* RenderGetOpenGLVersion()
{
    return (const char*)glGetString(GL_VERSION);
}
inline const char* RenderGetShaderVersion()
{
    return (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
}

inline int32 RenderGetDepthBufferSize()
{
    int32 bits = 0;
    glGetIntegerv(GL_DEPTH_BITS, &bits);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &bits);

    return bits;
}

inline int32 RenderGetStencilBufferSize()
{
    int32 bits = 0;
    glGetIntegerv(GL_STENCIL_BITS, &bits);

    return bits;
}

void RenderPrintInfo()
{
    LogWriteMessage("============================================\n");
    LogWriteMessage("Vendor : %s\n", RenderGetVendor());
    LogWriteMessage("OpenGL version : %s\n", RenderGetOpenGLVersion());
    LogWriteMessage("GLSL version : %s\n", RenderGetShaderVersion());
    LogWriteMessage("============================================\n");
}
