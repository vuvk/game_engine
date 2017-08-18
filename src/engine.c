#include "SDL2/SDL.h"

#include "defines.h"
#include "types.h"
#include "structs_private.h"
#include "engine.h"
#include "log_system.h"
#include "input_system.h"
#include "window_system.h"
#include "render_system.h"
#include "resource_manager.h"
#include "texture.h"
#include "material.h"
#include "font.h"
#include "image.h"
#include "text.h"
#include "node_2d.h"
#include "mesh_md2.h"
#include "node_md2.h"
#include "mesh_bsp.h"
#include "node_bsp.h"


const char ENGINE_NAME[]    = "bodhi Game Engine";
const char ENGINE_VERSION[] = "0.1.0.9";

static EEngineState _engineState = esNotRunning;

static uint64 _currTick;
static uint64 _lastTick;

// delta of time between last and current frame
static float _deltaTime;
// frames per seconds
static uint16 _fps;
// count of frames before _fpsDelay
static uint16 _frameCount;
// time before we calculate FPS
static float _fpsDelay;
// limit of frames per seconds
static uint16 _limitFPS;
static float _framerate;

int EngineStart(int32 windowWidth, int32 windowHeight, bool resizable, bool fullScreenMode)
{
    /* what do you want, if engine is already started? */
    if(_engineState == esRunning)
    {
        LogWriteWarning("Engine is already started!\n");
        return 0;
    }

    LogWriteMessage("============================================\n");
    LogWriteMessage("%s ver. %s\n", ENGINE_NAME, ENGINE_VERSION);
    LogWriteMessage("============================================\n");

    /* try to init SDL2 */
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        LogWriteError("Couldn't init SDL2! Error: %s\n", SDL_GetError());
        return ERROR_ENGINE_NOT_CREATED;
    }

    /* now engine is running */
    _engineState = esRunning;

    /* try to create window */
    int window_status = WindowCreate(windowWidth, windowHeight, resizable, fullScreenMode);
    if(window_status)
        return window_status;

    /* try to create OpenGL context */
    RenderCreate();

    LogWriteMessage("Engine started!\n");

    /* show info about system */
    RenderPrintInfo();

    /* timing */
    _currTick = SDL_GetTicks();
    _lastTick = _currTick;

    _deltaTime = 0.0f;
    _fps = 0;
    _fpsDelay = 0.0f;
    _limitFPS = 0;
    _framerate = 0.0f;

    /* initialize input system */
    InputClearTableOfPressedKeys();

    /* initialize resource manager */
    RM_Init();

    /* initialize containers */
    /* resources */
    _textures  = DictionaryCreate();
    _materials = DictionaryCreate();
    /* 2d */
    _images    = DictionaryCreate();
    _fonts     = DictionaryCreate();
    _texts     = DictionaryCreate();
    _nodes2d   = ListCreate();
    /* 3d */
    _meshesMd2 = DictionaryCreate();
    _nodesMd2  = ListCreate();

    return 0;
}

void EngineStop()
{
    WindowDestroy();
    SDL_Quit();

    _engineState = esNotRunning;

    /* clear resources */
    TexturesDestroyAll();
    MaterialsDestroyAll();
    ImagesDestroyAll();
    FontsDestroyAll();
    TextsDestroyAll();
    Nodes2DDestroyAll();
    MeshesMd2DestroyAll();
    NodesMd2DestroyAll();

    /* delete containers */
    DictionaryDestroy(&_textures);
    DictionaryDestroy(&_materials);
    DictionaryDestroy(&_images);
    DictionaryDestroy(&_fonts);
    DictionaryDestroy(&_texts);
    ListDestroy(&_nodes2d);

    DictionaryDestroy(&_meshesMd2);
    ListDestroy(&_nodesMd2);

    LogWriteWarning("Engine stopped!\n");
}

// timing
inline void EngineSetLimitFPS(uint16 limitFPS)
{
    _limitFPS = limitFPS;
    _framerate = 1000.0f / limitFPS;
}

inline uint16 EngineGetLimitFPS()
{
    return _limitFPS;
}

inline uint16 EngineGetFPS()
{
    return _fps;
}

inline float EngineGetDeltaTime()
{
    return _deltaTime;
}

void EngineUpdateTime()
{
    // calculate deltaTime
    _currTick = SDL_GetTicks();
    if(_currTick > _lastTick)
    {
        _deltaTime = (_currTick - _lastTick) / 1000.0f;
        _lastTick  =  _currTick;
    }
}

void EngineUpdateFPS()
{
    // если установлено ограничение FPS
    if(_limitFPS > 0.0f)
    {
        int delay =(int)SDL_floor(_framerate - _deltaTime);
        if(delay <= 0)
            delay = 1;
        // пропускаем разницу
        SDL_Delay(delay);
    }
    else
        SDL_Delay(1);

    // fps узнаем раз в полсекунды
    if(_fpsDelay < 0.5f)
    {
        // считаем количество кадров
        _frameCount++;
        // накапливаем задержку
        _fpsDelay += _deltaTime;
    }
    else
    {
        // фпс = секунда деленная на прошедшее время, деленное на количество кадров ))
        _fps = 1.0f /(_fpsDelay / _frameCount);
        _fpsDelay -= 0.5f;
        _frameCount = 0.0f;
    }
}

// system
inline EEngineState EngineGetStatus()
{
    return _engineState;
}

inline const char* EngineGetName()
{
    return ENGINE_NAME;
}

inline const char* EngineGetVersion()
{
    return ENGINE_VERSION;
}
