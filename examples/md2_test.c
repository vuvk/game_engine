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

    aTexture zumlinTxr = TextureCreateFromFile("jpg", "baseq2/models/monsters/zumlin/zumlin.pcx", true);
    aTexture railgunTxr = TextureCreateFromFile("railgun", "baseq2/models/monsters/zumlin/w_railgun.pcx", false);

    aMaterial zumlinMat = MaterialCreate("zumlin");
    MaterialSetTexture(zumlinMat, zumlinTxr);
    MaterialSetCullFace(zumlinMat, CF_FRONT);

    aMaterial railgunMat = MaterialCreate("railgun");
    MaterialSetTexture(railgunMat, railgunTxr);
    MaterialSetCullFace(railgunMat, CF_FRONT);
    aMaterial railgunMat_2 = MaterialCreate("railgun_2");
    MaterialSetTexture(railgunMat_2, railgunTxr);
    MaterialSetType(railgunMat_2, MT_ALPHA_BLENDED);
    MaterialSetCullFace(railgunMat, CF_FRONT);

    aMeshMd2 mesh = MeshMd2Create("zumlin!");
    MeshMd2LoadFromFile(mesh, "baseq2/models/monsters/zumlin/zumlin.md2");

    aMeshMd2 railgunMesh = MeshMd2Create("railgun");
    MeshMd2LoadFromFile(railgunMesh, "baseq2/models/monsters/zumlin/w_railgun.md2");

    aNodeMd2 nodeMd2 = NodeMd2Create("zumlin");
    NodeMd2AddLinkToMeshMd2(nodeMd2, mesh);
    NodeMd2SetMaterial(nodeMd2, zumlinMat);
    NodeMd2SetAnimSpeed(nodeMd2, -8.0f);
    //NodeMd2SetAnimLoop(nodeMd2, false);
    NodeMd2SetIntervalByName(nodeMd2, "stand");
    //NodeMd2SetInterval(nodeMd2, 95, 107);
    //NodeMd2SetCurrentFrame(nodeMd2, 107);
    NodeMd2SetPos(nodeMd2, -30, 0, 0);
    NodeMd2SetYawInDeg(nodeMd2, -90);
    //
    NodeMd2AddLinkToMeshMd2(nodeMd2, railgunMesh);
    NodeMd2SetMaterialEx(nodeMd2, railgunMat, 1);
    NodeMd2SetAnimSpeedEx(nodeMd2, -8.0f, 1);
    NodeMd2SetIntervalByNameEx(nodeMd2, "stand", 1);

    aNodeMd2 nodeMd2_2 = NodeMd2Create("zumlin");
    NodeMd2AddLinkToMeshMd2(nodeMd2_2, mesh);
    NodeMd2SetMaterial(nodeMd2_2, zumlinMat);
    //NodeMd2SetAnimLoop(nodeMd2_2, false);
    NodeMd2SetInterval(nodeMd2_2, 95, 107);
    NodeMd2SetPos(nodeMd2_2, 30, 0, 0);
    NodeMd2SetYawInDeg(nodeMd2_2, -90);
    //
    NodeMd2AddLinkToMeshMd2(nodeMd2_2, railgunMesh);
    NodeMd2SetMaterialEx(nodeMd2_2, railgunMat_2, 1);
    NodeMd2SetIntervalEx(nodeMd2_2, 95, 107, 1);

    aCamera cam = CameraCreate("test camera");
    CameraPlacef(cam, 0, 0, 100, 0, 0, 0, 0, 1, 0);

    while (!quit)
    {
        SceneSetClearColor(red, green, blue);
        SceneBegin();
        SceneDrawAllNodes();
        SceneEnd();

        /* keyboard test */
        if (InputIsKeyDown())
        {
            if (InputIsKeyPressed('1'))
                red = !red;

            if (InputIsKeyPressed('q'))
                TexturesDestroyAll();

            // manipulations with node
            if (InputIsKeyPressed('s'))
                NodeMd2SetPosZ(nodeMd2, NodeMd2GetPosZ(nodeMd2) + 1);

            if (InputIsKeyPressed('w'))
                NodeMd2SetPosZ(nodeMd2, NodeMd2GetPosZ(nodeMd2) - 1);

            if (InputIsKeyPressed('d'))
                NodeMd2SetYawInDeg(nodeMd2, RadToDeg(NodeMd2GetYaw(nodeMd2)) + 10);

            if (InputIsKeyPressed('a'))
                NodeMd2SetYawInDeg(nodeMd2, RadToDeg(NodeMd2GetYaw(nodeMd2)) - 10);

            if (InputIsKeyPressed(SDLK_z))
                NodeMd2SetScaleY(nodeMd2, NodeMd2GetScaleY(nodeMd2) - 0.1f);
            if (InputIsKeyPressed(SDLK_c))
                NodeMd2SetScaleY(nodeMd2, NodeMd2GetScaleY(nodeMd2) + 0.1f);

            if (InputIsKeyPressed(SDLK_v))
                MeshMd2SetScaleY(mesh, MeshMd2GetScaleY(mesh) - 0.1f);
            if (InputIsKeyPressed(SDLK_b))
                MeshMd2SetScaleY(mesh, MeshMd2GetScaleY(mesh) + 0.1f);

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
