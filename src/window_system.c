
#include "SDL2/SDL.h"


#include "log_system.h"
#include "window_system.h"
#include "render_system.h"
#include "defines.h"
#include "engine.h"


static SDL_Window* _sdlWindow;
static EWindowState _windowState = wsNotCreated;
static char* _windowTitle  = "";
static int32 _windowWidth;
static int32 _windowHeight;
static bool _fullScreenMode;

int WindowCreate(int32 windowWidth, int32 windowHeight, bool resizable, bool fullScreenMode)
{
    /* what do you want if engine is not started, hmm??*/
    if(EngineGetStatus() == esNotRunning)
    {
        LogWriteError("I can't create window if engine is not started!\n");
        return ERROR_WINDOW_NOT_CREATED;
    }

    /* what do you want, if window is already created? */
    if(_windowState == wsCreated)
    {
        LogWriteWarning("Window is already created!\n");
        return 0;
    }

    /* set SDL attributes */
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,         8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,       1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,         24);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,        32);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,     8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,    8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    /* try to create window */
    int windowFlags = SDL_WINDOW_OPENGL;
    if(resizable)
        windowFlags |= SDL_WINDOW_RESIZABLE;

    _sdlWindow = SDL_CreateWindow(_windowTitle,
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  windowWidth, windowHeight,
                                  windowFlags);
    if(!_sdlWindow)
    {
        LogWriteError("Couldn't create window! Error: %s\n", SDL_GetError());
        return ERROR_WINDOW_NOT_CREATED;
    }

    /* set full screen, if need */
    if(fullScreenMode)
        SDL_SetWindowFullscreen(_sdlWindow, SDL_WINDOW_FULLSCREEN);

    /* initialization */
    _windowTitle    = "";
    _windowWidth    = windowWidth;
    _windowHeight   = windowHeight;
    _windowState    = wsCreated;
    _fullScreenMode = fullScreenMode;

    return 0;
}

void WindowDestroy()
{
    RenderDestroy();

    SDL_DestroyWindow(_sdlWindow);
    _sdlWindow = NULL;

    _windowState = wsNotCreated;
}

inline void WindowSetSize(int32 windowWidth, int32 windowHeight)
{
    SDL_SetWindowSize(_sdlWindow, windowWidth, windowHeight);
    _windowWidth = windowWidth;
    _windowHeight = windowHeight;
}

inline void WindowSetTitle(const char* windowTitle)
{
    SDL_SetWindowTitle(_sdlWindow, windowTitle);
}

inline void WindowSetFullScreenMode(bool fullScreenMode, bool useDesktopResolution)
{
    if(fullScreenMode)
    {
        if(!useDesktopResolution)
            SDL_SetWindowFullscreen(_sdlWindow, SDL_WINDOW_FULLSCREEN);
        else
            SDL_SetWindowFullscreen(_sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else
        SDL_SetWindowFullscreen(_sdlWindow, 0);
}
//void EngineSetWindowResizable(bool isResizable);

//windows gets
inline void WindowGetSize(int32* windowWidth, int32* windowHeight)
{
    if(windowWidth && windowHeight)
        SDL_GetWindowSize(_sdlWindow, windowWidth, windowHeight);
}

inline const char* WindowGetTitle()
{
    return _windowTitle;
}

inline int WindowGetWidth()
{
    return _windowWidth;
}

inline int WindowGetHeight()
{
    return _windowHeight;
}

// system
void WindowUpdateState()
{
    static int32 prevWindowWidth  = 0;
    static int32 prevWindowHeight = 0;

    if(_sdlWindow != NULL)
    {
        SDL_GetWindowSize(_sdlWindow, &_windowWidth, &_windowHeight);

        if ((_windowWidth  != prevWindowWidth) ||
            (_windowHeight != prevWindowHeight))
        {
            Perspectivef_M4x4(_pers, 60.0f, (float)_windowWidth / _windowHeight, 0.1f, 1000.0f);

            prevWindowWidth  = _windowWidth;
            prevWindowHeight = _windowHeight;
        }
    }
    else
    {
        _windowTitle  = "";
        _windowWidth  = 0;
        _windowHeight = 0;
        _windowState  = wsNotCreated;
    }
}

inline EWindowState WindowGetState()
{
    return _windowState;
}

inline SDL_Window* WindowGetPointer()
{
    return _sdlWindow;
}
