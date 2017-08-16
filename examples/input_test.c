#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "SDL2/SDL.h"

#include "all_in_one.h"

int main(int args, char* argv[])
{
    setlocale (LC_ALL, "Russian");

    LogSetOutputFile("log.log");
    LogSetWritingToOutputFile(true);

    EngineStart(800, 600, true, false);
    EngineSetLimitFPS(60);
    WindowSetTitle(EngineGetName());

    bool red = 1, green = 0, blue = 0;
    bool quit = false;
    char title[50] = "";
    uint16 curFPS = 0;
    uint16 prevFPS = 0;

    aTexture fontTxr = TextureCreateFromFile("font", "baseq2/fonts/font.png", false);
    aFont font = FontCreate("test font");
    FontGenerateFromTexture(font, fontTxr, 16, 16, 6, 0);

    SVector2i mousePos = {0};

    while (!quit)
    {
        SceneSetClearColor(red, green, blue);
        SceneBegin();

        GuiBegin();
        if (InputIsMouseButtonMiddleHeld())
        {
            mousePos = InputGetMousePos();
            GuiDrawText(font, mousePos.x, mousePos.y, "+");
        }
        GuiEnd();

        SceneEnd();

        /* keyboard test */
        if (InputIsKeyDown())
        {
            if (InputIsKeyPressed('1'))
                red = !red;

            SDL_Keycode key = InputGetLastKeyCode();
            switch (key)
            {
                case '2' :
                {
                    green = !green;
                    break;
                }
                case SDLK_3 :
                {
                    blue = !blue;
                    break;
                }

                case SDLK_ESCAPE :
                {
                    quit = true;
                    continue;
                }

                default : break;
            }
        }

        /* mouse tests */
        if (InputIsMouseButtonUp())
        {
            LogWriteMessage("mouse up!\n");
            SVector2i mousePos = InputGetMousePos();
            LogWriteMessage("mouse position is x:%3d y:%3d\n", mousePos.x, mousePos.y);

            if (InputIsMouseButtonLeftClicked())
                LogWriteMessage("mouse left clicked!\n");
            if (InputIsMouseButtonRightClicked())
                LogWriteMessage("mouse right clicked!\n");
            if (InputIsMouseButtonMiddleClicked())
                LogWriteMessage("mouse middle clicked!\n");

            LogWriteMessage("-------------------------------\n");
        }

        if (InputIsMouseButtonDown())
        {
            LogWriteMessage("-------------------------------\n");
            LogWriteMessage("mouse down!\n");
        }

        if (InputIsMouseWheel())
        {
            if (InputIsMouseWheelUp())
                LogWriteMessage("mouse wheel up!\n");
            if (InputIsMouseWheelDown())
                LogWriteMessage("mouse wheel down!\n");
        }

        // write FPS in window's title
        curFPS = EngineGetFPS();
        if (curFPS != prevFPS)
        {
            sprintf(title, "%s (v. %s) FPS : %d", EngineGetName(), EngineGetVersion(), EngineGetFPS());
            WindowSetTitle(title);
        }
        prevFPS = curFPS;
    }

    EngineStop();

    return 0;
}
