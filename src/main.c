#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "SDL2/SDL.h"

#include "all_in_one.h"
#include "structs_private.h"

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

/*
    char str[5] = {0};

    uint32 numberTXR = 0x5F525854; // 1599232084 = 0x5F525854
    StrCopy(str, (char*)&numberTXR, 5);
    printf("TXR_ = %s\n", str);
    char TXR[4] = {"TXR_"};
    printf("TXR_ - %d\n\n", *((uint32*)TXR));

    uint32 numberMAT = 0x5F54414D; // 1599357261 = 0x5F54414D
    StrCopy(str, (char*)&numberMAT, 5);
    printf("MAT_ = %s\n", str);
    char MAT[4] = {"MAT_"};
    printf("MAT_ - %d\n\n", *((uint32*)MAT));

    uint32 numberIMG = 0x5F474D49; // 1598508361 = 0x5F474D49
    StrCopy(str, (char*)&numberIMG, 5);
    printf("IMG_ = %s\n", str);
    char IMG[4] = {"IMG_"};
    printf("IMG_ - %d\n\n", *((uint32*)IMG));

    uint32 numberFNT = 0x5F544E46; // 1599360582 = 0x5F544E46
    StrCopy(str, (char*)&numberFNT, 5);
    printf("FNT_ = %s\n", str);
    char FNT[4] = {"FNT_"};
    printf("FNT_ - %d\n\n", *((uint32*)FNT));

    uint32 numberTXT = 0x5F545854; // 1599363156 = 0x5F545854
    StrCopy(str, (char*)&numberTXT, 5);
    printf("TXT_ = %s\n", str);
    char TXT[4] = {"TXT_"};
    printf("TXT_ - %d\n\n", *((uint32*)TXT));

    uint32 numberN_2D = 0x44325F4E; // 1144151886 = 0x44325F4E
    StrCopy(str, (char*)&numberN_2D, 5);
    printf("N_2D = %s\n", str);
    char N_2D[4] = {"N_2D"};
    printf("N_2D - %d\n\n", *((uint32*)N_2D));

    uint32 numberMD2 = 0x5F32444D; // 1597129805 = 0x5F32444D
    StrCopy(str, (char*)&numberMD2, 5);
    printf("MD2_ = %s\n", str);
    char MD2[4] = {"MD2_"};
    printf("MD2_ - %d\n\n", *((uint32*)MD2));

    uint32 IDP2 = 0x32504449;  //  844121161 = 0x32504449
    StrCopy(str, (char*)&IDP2, 5);
    printf("IDP2 = %s\n\n", str);

    uint32 numberNodeMD2 = 0x32444D4E; // 843337038 = 0x32444D4E
    StrCopy(str, (char*)&numberNodeMD2, 5);
    printf("NMD2 = %s\n", str);
    char NMD2[4] = {"NMD2"};
    printf("NMD2 - %d\n\n", *((uint32*)NMD2));

    uint32 numberCam = 0x5F4D4143; // 1598898499 = 0x5F4D4143
    StrCopy(str, (char*)&numberCam, 5);
    printf("CAM_ = %s\n", str);
    char CAM[4] = {"CAM_"};
    printf("CAM_ - %d\n\n", *((uint32*)CAM));
*/

#ifdef _DEBUG
    //utils test. Да ну?!
    //UtilsTest();

    //list test
    //ListTest();

    // dictionary test
    //DictionaryTest();

    // textures test
    //TextureTest();

    // materials test
    //MaterialTest();

    // images test
    //ImageTest();

    // fonts test
    //FontTest();

    // texts test
    //TextTest();
#endif // DEBUG

    RenderPrintScreenModes();

    aCamera cam = CameraCreate("test camera");
    CameraPlacef(cam, 0, 0, 0, 0, 0, 0, 0, 1, 0);

    bool rotL    = false, rotR    = false;
    bool moveF   = false, moveB   = false;
    bool strafeL = false, strafeR = false;

    SMeshBsp* meshBsp = calloc(1, sizeof(SMeshBsp));
    RM_LoadMeshBsp("baseq2/maps/base1.bsp", &meshBsp);

    while (!quit)
    {
        if (InputIsMouseMove())
            CameraSetViewByMouse(cam);

        if (moveF)   CameraMove(cam,  100.0f*EngineGetDeltaTime());
        if (moveB)   CameraMove(cam, -100.0f*EngineGetDeltaTime());
        if (rotL)    CameraRotatev(cam, DegToRad( 100.0f*EngineGetDeltaTime()), vUp3f);
        if (rotR)    CameraRotatev(cam, DegToRad(-100.0f*EngineGetDeltaTime()), vUp3f);
        if (strafeR) CameraStrafe(cam,  100.0f*EngineGetDeltaTime());
        if (strafeL) CameraStrafe(cam, -100.0f*EngineGetDeltaTime());

        SceneSetClearColor(red, green, blue);
        SceneBegin();
        SceneDrawAllNodes();
        SceneEnd();

        /* keyboard test */
        if (InputIsKeyDown())
        {
            if (InputIsKeyPressed('1'))
                red = !red;
            if (InputIsKeyPressed('2'))
                green = !green;
            if (InputIsKeyPressed('3'))
                blue = !blue;

            /* CAMERA */
            if (InputIsKeyPressed('w')) moveF = true;
            if (InputIsKeyPressed('s')) moveB = true;

            if (InputIsKeyPressed('q')) rotL = true;
            if (InputIsKeyPressed('e')) rotR = true;

            if (InputIsKeyPressed('a')) strafeL = true;
            if (InputIsKeyPressed('d')) strafeR = true;

            if (InputIsKeyPressed(SDLK_ESCAPE))
            {
                quit = true;
                continue;
            }
        }

        if (InputIsKeyUp())
        {
            /* CAMERA */
            if (!InputIsKeyPressed('w')) moveF = false;
            if (!InputIsKeyPressed('s')) moveB = false;

            if (!InputIsKeyPressed('q')) rotL = false;
            if (!InputIsKeyPressed('e')) rotR = false;

            if (!InputIsKeyPressed('a')) strafeL = false;
            if (!InputIsKeyPressed('d')) strafeR = false;
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
