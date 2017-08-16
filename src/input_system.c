#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL2/SDL.h"

#include "utils.h"
#include "input_system.h"
#include "window_system.h"

static SDL_Event _sdlEvent;

/* keyboard state */
#define KEYBOARD_KEY_DOWN   (1 << 0)
#define KEYBOARD_KEY_UP     (1 << 1)
static uint8 _keyboardState;
static SDL_Keycode  _lastKeyCode;
static SDL_Scancode _lastScanCode;
// see http://wiki.libsdl.org/SDLScancodeLookup for details
#define MAX_KEY_TABLE_SIZE SDL_NUM_SCANCODES
/* table of pressed keys */
/* таблица из нажатых в данный момент кнопок */
static bool _keysPressed [MAX_KEY_TABLE_SIZE] = {0};

/* mouse state */
#define MOUSE_MOVE          (1 << 0)
#define MOUSE_BUTTON_UP     (1 << 1)    /* any button was unpressed */
#define MOUSE_BUTTON_DOWN   (1 << 2)    /* any button was pressed */
#define MOUSE_WHEEL         (1 << 3)
#define MOUSE_LAST_LEFT     (1 << 4)    /* last pressed mouse button - left   */
#define MOUSE_LAST_RIGHT    (1 << 5)    /* last pressed mouse button - right  */
#define MOUSE_LAST_MIDDLE   (1 << 6)    /* last pressed mouse button - middle */
#define MOUSE_HELD_LEFT     (1 << 7)    /* left button is held down   */
#define MOUSE_HELD_RIGHT    (1 << 8)    /* right button is held down  */
#define MOUSE_HELD_MIDDLE   (1 << 9)    /* middle button is held down */
static SVector2i _mousePosition;
static int32  _mouseWheelDirection;
static uint16 _mouseState;

static void InputClearState()
{
    /* drop keyboard state */
    _keyboardState = 0;
    _lastKeyCode = -1;

    /* clear mouse state */
    //_mouseState = 0;
    if (_mouseState & MOUSE_MOVE)
        _mouseState &= ~MOUSE_MOVE;

    if (_mouseState & MOUSE_BUTTON_UP)
        _mouseState &= ~MOUSE_BUTTON_UP;

    if (_mouseState & MOUSE_BUTTON_DOWN)
        _mouseState &= ~MOUSE_BUTTON_DOWN;

    if (_mouseState & MOUSE_WHEEL)
        _mouseState &= ~MOUSE_WHEEL;

    if (_mouseState & MOUSE_LAST_LEFT)
        _mouseState &= ~MOUSE_LAST_LEFT;

    if (_mouseState & MOUSE_LAST_RIGHT)
        _mouseState &= ~MOUSE_LAST_RIGHT;

    if (_mouseState & MOUSE_LAST_MIDDLE)
        _mouseState &= ~MOUSE_LAST_MIDDLE;

    _mouseWheelDirection = 0;
}

void InputUpdateState()
{
    InputClearState();

    /* if exists any event */
    while(SDL_PollEvent(&_sdlEvent) == 1)
    {
        /* save keyboard state */
        if (_sdlEvent.type == SDL_KEYDOWN ||
            _sdlEvent.type == SDL_KEYUP)
        {
            _lastKeyCode = _sdlEvent.key.keysym.sym;
            _lastScanCode = _sdlEvent.key.keysym.scancode;

            /* mark key as pressed if key is down... and as not pressed if key is up */
            _keysPressed[_lastScanCode] = (_sdlEvent.type == SDL_KEYDOWN);
        }

        if(_sdlEvent.type == SDL_KEYDOWN)
            _keyboardState |= KEYBOARD_KEY_DOWN;

        if(_sdlEvent.type == SDL_KEYUP)
            _keyboardState |= KEYBOARD_KEY_UP;


        /* save mouse state */
        if (_sdlEvent.type == SDL_MOUSEMOTION)
            _mouseState |= MOUSE_MOVE;

        if (_sdlEvent.type == SDL_MOUSEWHEEL)
        {
            _mouseState |= MOUSE_WHEEL;
            _mouseWheelDirection = _sdlEvent.wheel.y;
        }

        if (_mouseState & MOUSE_MOVE)
            SDL_GetMouseState(&_mousePosition.x, &_mousePosition.y);

        if (_sdlEvent.type == SDL_MOUSEBUTTONDOWN)
        {
            _mouseState &= ~MOUSE_BUTTON_UP;
            _mouseState |=  MOUSE_BUTTON_DOWN;
        }

        if (_sdlEvent.type == SDL_MOUSEBUTTONUP)
        {
            _mouseState &= ~MOUSE_BUTTON_DOWN;
            _mouseState |=  MOUSE_BUTTON_UP;
        }

        if (_sdlEvent.button.button == SDL_BUTTON_LEFT)
        {
            _mouseState |= MOUSE_LAST_LEFT;

            /* удержание кнопки */
            if (_mouseState & MOUSE_BUTTON_DOWN)
                _mouseState |= MOUSE_HELD_LEFT;
            if (_mouseState & MOUSE_BUTTON_UP)
                _mouseState &= ~MOUSE_HELD_LEFT;
        }

        if (_sdlEvent.button.button == SDL_BUTTON_RIGHT)
        {
            _mouseState |= MOUSE_LAST_RIGHT;

            /* удержание кнопки */
            if (_mouseState & MOUSE_BUTTON_DOWN)
                _mouseState |= MOUSE_HELD_RIGHT;
            if (_mouseState & MOUSE_BUTTON_UP)
                _mouseState &= ~MOUSE_HELD_RIGHT;
        }

        if (_sdlEvent.button.button == SDL_BUTTON_MIDDLE)
        {
            _mouseState |= MOUSE_LAST_MIDDLE;

            /* удержание кнопки */
            if (_mouseState & MOUSE_BUTTON_DOWN)
                _mouseState |= MOUSE_HELD_MIDDLE;
            if (_mouseState & MOUSE_BUTTON_UP)
                _mouseState &= ~MOUSE_HELD_MIDDLE;
        }
    }
}

/*----------*/
/* KEYBOARD */
/*----------*/
inline bool InputIsKeyDown()
{
    return (_keyboardState & KEYBOARD_KEY_DOWN);
}

inline bool InputIsKeyUp()
{
    return (_keyboardState & KEYBOARD_KEY_UP);
}

inline bool InputIsKeyPressed(SDL_Keycode keyCode)
{
    return _keysPressed[SDL_GetScancodeFromKey(keyCode)];
}

inline SDL_Keycode InputGetLastKeyCode()
{
    return _lastKeyCode;
}

inline SDL_Scancode InputGetLastScanCode()
{
    return _lastScanCode;
}

inline void InputClearTableOfPressedKeys()
{
    memset(&_keysPressed, 0, MAX_KEY_TABLE_SIZE*sizeof(bool));
}


/*-------*/
/* MOUSE */
/*-------*/
inline bool InputIsMouseMove()
{
    return (_mouseState & MOUSE_MOVE);
}

inline bool InputIsMouseButtonDown()
{
    return (_mouseState & MOUSE_BUTTON_DOWN);
}

inline bool InputIsMouseButtonUp()
{
    return (_mouseState & MOUSE_BUTTON_UP);
}

inline SVector2i InputGetMousePos()
{
    return _mousePosition;
}

inline int32 InputGetMousePosX()
{
    return _mousePosition.x;
}

inline int32 InputGetMousePosY()
{
    return _mousePosition.y;
}

inline bool InputIsMouseWheel()
{
    return (_mouseState & MOUSE_WHEEL);
}

inline int32 InputGetMouseWheelDir()
{
    return _mouseWheelDirection;
}

inline bool InputIsMouseWheelUp()
{
    return (_mouseWheelDirection > 0);
}

inline bool InputIsMouseWheelDown()
{
    return (_mouseWheelDirection < 0);
}

inline void InputSetMousePosi(int32 mousePositionX, int32 mousePositionY)
{
    _mousePosition.x = mousePositionX;
    _mousePosition.y = mousePositionY;
    SDL_WarpMouseInWindow(WindowGetPointer(), _mousePosition.x, _mousePosition.y);
}

inline void InputSetMousePosv(SVector2i mousePosition)
{
    _mousePosition = mousePosition;
    SDL_WarpMouseGlobal(_mousePosition.x, _mousePosition.y);
}

inline void InputSetMousePosX(int32 mousePositionX)
{
    _mousePosition.x = mousePositionX;
    SDL_WarpMouseGlobal(_mousePosition.x, _mousePosition.y);
}

inline void InputSetMousePosY(int32 mousePositionY)
{
    _mousePosition.y = mousePositionY;
    SDL_WarpMouseGlobal(_mousePosition.x, _mousePosition.y);
}

inline bool InputIsMouseButtonLeftClicked()
{
    return (_mouseState & MOUSE_LAST_LEFT);
}

inline bool InputIsMouseButtonRightClicked()
{
    return (_mouseState & MOUSE_LAST_RIGHT);
}

inline bool InputIsMouseButtonMiddleClicked()
{
    return (_mouseState & MOUSE_LAST_MIDDLE);
}

inline bool InputIsMouseButtonLeftHeld()
{
    return (_mouseState & MOUSE_HELD_LEFT);
}

inline bool InputIsMouseButtonRightHeld()
{
    return (_mouseState & MOUSE_HELD_RIGHT);
}

inline bool InputIsMouseButtonMiddleHeld()
{
    return (_mouseState & MOUSE_HELD_MIDDLE);
}
