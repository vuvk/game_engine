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


    // draw 2d in low-level test
    aTexture pcxTxr = TextureCreateFromFile("pcx", "baseq2/models/monsters/infantry/skin.pcx", false);
    aImage pcxImg = ImageCreate("pcx image");
    ImageSetTexture(pcxImg, pcxTxr);
    ImageSetPivot(pcxImg, ImageGetWidth(pcxImg)/2, ImageGetHeight(pcxImg)/2);

    aTexture logoTxr = TextureCreateFromFile("logo", "baseq2/textures/logo2.png", false);
    aImage logoImg = ImageCreate("logo image");
    ImageSetTexture(logoImg, logoTxr);
    ImageSetPivot(logoImg, ImageGetWidth(logoImg)/2, ImageGetHeight(pcxImg));

    aTexture fontTxr = TextureCreateFromFile("font", "baseq2/fonts/font.png", false);
    aFont font = FontCreate("test font");
    FontGenerateFromTexture(font, fontTxr, 16, 16, 6, 0);

    aText text = TextCreate("test text", font, "Test Text");
    TextSetPivot(text, (StrLength(TextGetString(text))*FontGetCharWidth(font)) / 2.0f, 100.0f);

    aNode2D node = Node2DCreate("first node!");
    Node2DAddLinkToImage(node, logoImg);
    Node2DAddLinkToImage(node, pcxImg);
    Node2DAddLinkToText(node, text);
    Node2DSetPos(node, WindowGetWidth()/2, WindowGetHeight()/2.0f);
    //Node2DSetVisible(node, false);

    SVector2i mousePos = {0};
    float angle = 0.0f;

    while (!quit)
    {
        SceneSetClearColor(red, green, blue);
        SceneBegin();
        SceneDrawAllNodes();

        // gui manually test
        if (!Node2DIsVisible(node))
        {
            GuiBegin();
            GuiDrawImage(logoImg, WindowGetWidth()/2.0f, WindowGetHeight() + 100.0f);
            GuiDrawImage(pcxImg,  WindowGetWidth()/2.0f, WindowGetHeight()/2.0f);
            GuiDrawText(font, 10.0f, 10.0f, "\tPRIVET\nSTALEVARAM\n\tI\nKVAKERAM!\n\n...aNoThEr TeXt...");
            if (InputIsMouseButtonMiddleHeld())
            {
                mousePos = InputGetMousePos();
                GuiDrawText(font, mousePos.x, mousePos.y, "+");
            }
            GuiEnd();
        }

        Node2DSetRotationInDeg(node, angle);
        angle += 10*EngineGetDeltaTime();
        if (angle > 360.0f)
            angle -= 360.0f;

        SceneEnd();

        /* keyboard test */
        if (InputIsKeyDown())
        {
            if (InputIsKeyPressed('1'))
                red = !red;

            if (InputIsKeyPressed('q'))
                TexturesDestroyAll();

            if (InputIsKeyPressed(SDLK_LEFT))
            {
                ImageSetWidth(pcxImg, ImageGetWidth(pcxImg) - 5);
                ImageSetPivot(pcxImg, ImageGetWidth(pcxImg)/2.0f, ImageGetHeight(pcxImg)/2.0f);
            }

            if (InputIsKeyPressed(SDLK_RIGHT))
            {
                ImageSetWidth(pcxImg, ImageGetWidth(pcxImg) + 5);
                ImageSetPivot(pcxImg, ImageGetWidth(pcxImg)/2.0f, ImageGetHeight(pcxImg)/2.0f);
            }

            if (InputIsKeyPressed(SDLK_UP))
            {
                ImageSetHeight(pcxImg, ImageGetHeight(pcxImg) - 5);
                ImageSetPivot(pcxImg, ImageGetWidth(pcxImg)/2.0f, ImageGetHeight(pcxImg)/2.0f);
            }

            if (InputIsKeyPressed(SDLK_DOWN))
            {
                ImageSetHeight(pcxImg, ImageGetHeight(pcxImg) + 5);
                ImageSetPivot(pcxImg, ImageGetWidth(pcxImg)/2.0f, ImageGetHeight(pcxImg)/2.0f);
            }

            if (InputIsKeyPressed(SDLK_HOME))
                Node2DSetPosY(node, Node2DGetPosY(node) - 5.0f);

            if (InputIsKeyPressed(SDLK_END))
                Node2DSetPosY(node, Node2DGetPosY(node) + 5.0f);

            if (InputIsKeyPressed(SDLK_DELETE))
                Node2DSetPosX(node, Node2DGetPosX(node) - 5.0f);

            if (InputIsKeyPressed(SDLK_PAGEDOWN))
                Node2DSetPosX(node, Node2DGetPosX(node) + 5.0f);
				
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

                case SDLK_INSERT :
                {
                    Node2DSetVisible(node, !Node2DIsVisible(node));
                    break;
                }

                case SDLK_PAGEUP:
                {
                    TextSetString(text, "Marana Malaxus!");
                    TextSetPivot(text, (StrLength(TextGetString(text))*FontGetCharWidth(font)) / 2.0f, 100.0f);
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
