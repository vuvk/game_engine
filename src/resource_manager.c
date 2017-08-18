#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include "structs_private.h"
#include "types.h"
#include "utils.h"
#include "color.h"
#include "log_system.h"
#include "file_system.h"
#include "render_system.h"
#include "resource_manager.h"

#include "format_md2.h"
#include "format_bsp.h"
#include "mesh_md2.h"
#include "mesh_bsp.h"

#define _IL_VERSION
//#define _SDL_VERSION

#ifdef _IL_VERSION
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"
#endif // _IL_VERSION

#include "SDL2/SDL.h"
#ifdef _SDL_VERSION
#include "SDL2/SDL_image.h"
#endif // _SDL_VERSION


#ifdef _SDL_VERSION
static SDL_Surface* _ethalonSurface;
#endif // _SDL_VERSION


#ifdef _IL_VERSION
// return true if error
static bool CheckILError()
{
    uint32 err = ilGetError();
    if (err != IL_NO_ERROR)
    {
        LogWriteError(ilGetString(err));
        return true;
    }

    return false;
}
#endif // _IL_VERSION

#ifdef _SDL_VERSION
// return true if error
static bool CheckSDLError()
{
    const char* err = SDL_GetError();
    if (strlen(err) != 0)
    {
        LogWriteError("%s\n", err);
        return true;
    }

    return false;
}
#endif //_SDL_VERSION


bool RM_Init()
{
#ifdef _IL_VERSION
    // version with IL

    if (ilGetInteger (IL_VERSION_NUM) < IL_VERSION)
        LogWriteWarning("Update your DevIL library!\n--Version of DevIL installed on system - %d. Engine created with - %d\n", ilGetInteger (IL_VERSION_NUM), IL_VERSION);

    if (iluGetInteger (ILU_VERSION_NUM) < ILU_VERSION)
        LogWriteWarning("Update your ilu library!\n--Version of ilu installed on system - %d. Engine created with - %d\n", iluGetInteger (ILU_VERSION_NUM), ILU_VERSION);

    if (ilutGetInteger (ILUT_VERSION_NUM) < ILUT_VERSION)
        LogWriteWarning("Update your ilut library!\n--Version of ilut installed on system - %d. Engine created with - %d\n", ilutGetInteger (ILUT_VERSION_NUM), ILUT_VERSION);

    ilInit();
    iluInit();
    ilutInit();

    ilutRenderer (ILUT_OPENGL);
    ilSetInteger (IL_KEEP_DXTC_DATA, IL_TRUE);
    ilutEnable (ILUT_GL_AUTODETECT_TEXTURE_TARGET);
    ilutEnable (ILUT_OPENGL_CONV);
    ilutEnable (ILUT_GL_USE_S3TC);

    if (CheckILError())
        return false;

    return true;
#endif // _IL_VERSION



#ifdef _SDL_VERSION
    // version with SDL2_image

    IMG_Init(IMG_INIT_JPG);
    IMG_Init(IMG_INIT_PNG);

    /* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
       as expected by OpenGL for textures */
    Uint32 rmask, gmask, bmask, amask;

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    _ethalonSurface = SDL_CreateRGBSurface(0, 1, 1, 32,
                                           rmask, gmask, bmask, amask);
    if (_ethalonSurface == NULL)
    {
        LogWriteError("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        return false;
    }

    return true;
#endif // _SDL_VERSION
}

int32 RM_LoadGLTexture (const char* fileName, int32* imgWidth, int32* imgHeight, bool buildMipMaps)
{

#ifdef _IL_VERSION
    // version with IL
    if (!FileExists(fileName))
    {
        LogWriteError("File '%s' doesn't exist!\n", fileName);
        return 0;
    }

    uint8* buffer = NULL;
    int32  bufSize = FileLoad(fileName, (void**)(&buffer));
    if (bufSize <= 0)
        return 0;
    char* ext = NULL;
    FileExtractExt(&ext, fileName);

    uint32 txrId = 0;

    ilGenImages (1, &txrId);
    ilBindImage (txrId);

    // определяем формат изображения по расширению
    ILenum imageType = IL_TYPE_UNKNOWN;
    if (StrEqual(ext, "PCX") == true)
        imageType = IL_PCX;
    if (StrEqual(ext, "JPG") == true || StrEqual(ext, "JPEG") == true)
        imageType = IL_JPG;
    if (StrEqual(ext, "TGA") == true)
        imageType = IL_TGA;
    if (StrEqual(ext, "WAL") == true)
        imageType = IL_WAL;

    ilLoadL(imageType, (const void*)buffer, (uint32)bufSize);
    if (CheckILError())
        return 0;

    // подтягиваем размер изображения
    if (imgWidth != NULL)
        *imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
    if (imgHeight != NULL)
        *imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);

    // TODO : действительно ли загружается изображение перевернутым или это я накосячил с отрисовкой 2D-прямоугольников?
    iluFlipImage ();
    uint32 glTexture = ilutGLBindTexImage();
    if (buildMipMaps)
        ilutGLBuildMipmaps();

    ilDeleteImages (1, &txrId);

    free(buffer);
    buffer = NULL;

    return glTexture;
#endif // _IL_VERSION



#ifdef _SDL_VERSION
    // version with SDL2_image

    if (!FileExists(fileName))
    {
        LogWriteError("File '%s' doesn't exist!\n", fileName);
        return 0;
    }

    char* fileExt;
    FileExtractExt(&fileExt, fileName);
    SDL_Surface* sdlSurface = NULL;

    sdlSurface = IMG_Load(fileName);
    CheckSDLError();

    if (sdlSurface == NULL)
    {
        LogWriteError("'%s' is not compatible or file '%s' is corrupt!\n", fileExt, fileName);
        return 0;
    }

    // преобразовываем все картинки в формат для OpenGL
    sdlSurface = SDL_ConvertSurface(sdlSurface, _ethalonSurface->format, 0);

    uint32 glTexture = 0;
    if (imgWidth)
        *imgWidth = sdlSurface->w;
    if (imgHeight)
        *imgHeight = sdlSurface->h;

    // генерим текстуру и возвращаем на неё идентификатор
    glGenTextures(1, &glTexture);
    glBindTexture(GL_TEXTURE_2D, glTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (buildMipMaps)
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, sdlSurface->w, sdlSurface->h, GL_RGBA, GL_UNSIGNED_BYTE, sdlSurface->pixels);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sdlSurface->w, sdlSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, sdlSurface->pixels);

    SDL_FreeSurface(sdlSurface);

    return glTexture;
#endif //_SDL_VERSION
}


bool RM_LoadMeshMd2(const char* fileName, void** meshMd2)
{
    if (!FileExists(fileName))
    {
        LogWriteError("File '%s' doesn't exist!\n", fileName);
        return false;
    }

    if (meshMd2 == NULL || !ObjectIsMeshMd2(*meshMd2))
        return false;

    SMeshMd2* mesh = (SMeshMd2*)*meshMd2;
    uint8* buffer = NULL;
    int32  bufSize = FileLoad(fileName, (void**)(&buffer));
    bool isLoaded = false;
    if (bufSize <= 0)
    {
        LogWriteError("File '%s' is not loaded!\n", fileName);
        return false;
    }

    // first we load model md2 to memory
    SModelMd2* mdl = calloc(1, sizeof(SModelMd2));
    if (mdl == NULL)
        goto result;

	// Read the header data
	memcpy(&(mdl->header), buffer, sizeof(SHeaderMd2));

	// .Md2 files MUST have a version of 8 and magic word IDP2
	if (mdl->header.version != 8 ||
        mdl->header.ident   != 0x32504449)     // 'I' 'D' 'P' '2' = 0x32504449
	{
		// Display an error message for bad file format, then stop loading
		LogWriteError("Invalid file format: %s!\n", fileName);
		goto result;
	}

    // =============читаем данные по модели===============
    // читаем шкурки (путь до них)
    // read skins
    if (mdl->header.numOfSkins > 0)
    {
        mdl->skins = calloc(1, mdl->header.numOfSkins*sizeof(TSkinMd2));
        if (mdl->skins == NULL)
            goto result;
        memcpy(mdl->skins, buffer + mdl->header.offsetSkins, mdl->header.numOfSkins*sizeof(TSkinMd2));
    }

    // полигоны
    // read tris
    if (mdl->header.numOfTris > 0)
    {
        mdl->triangles = calloc(1, mdl->header.numOfTris*sizeof(STriangleMd2));
        if (mdl->triangles == NULL)
            goto result;
        memcpy(mdl->triangles, buffer + mdl->header.offsetTris, mdl->header.numOfTris*sizeof(STriangleMd2));
    }

    // читаем кадры
    // read frames
    if (mdl->header.numOfFrames > 0)
    {
        /* Memory allocation for frames */
        mdl->frames = calloc(1, mdl->header.numOfFrames*sizeof(SFrameMd2));
        if (mdl->frames == NULL)
            goto result;

        /* pointer where read frame's info */
        uint8* ptrForRead = buffer + mdl->header.offsetFrames;
        /* size of parameters for one frame */
        int32  sizeOfFrameParam = sizeof(SVector3f) +                            /* scale */
                                  sizeof(SVector3f) +                            /* translate */
                                  MD2_MAX_FRAME_NAME*sizeof(char);               /* name */
        /* size of vertices in one frame */
        int32  sizeOfFrameVerts = mdl->header.numOfVertices*sizeof(SVertexMd2);  /* vertices */
        /* size of one frame */
        int32  sizeOfFrame      = sizeOfFrameParam + sizeOfFrameVerts;

        for (int32 i = 0; i < mdl->header.numOfFrames; ++i)
        {
            /* Memory allocation for vertices of this frame */
            mdl->frames[i].verts = calloc(1, sizeOfFrameVerts);
            if (mdl->frames[i].verts == NULL)
                goto result;

            /* Read frame data */
            memcpy(&(mdl->frames[i]), ptrForRead, sizeOfFrameParam);
            memcpy(mdl->frames[i].verts, ptrForRead + sizeOfFrameParam, sizeOfFrameVerts);
            ptrForRead += sizeOfFrame;
        }
    }

    // текстурные координаты
    // read tex. coords
    if (mdl->header.numOfSt > 0)
    {
        // The texture coordinates from file
        STexCoordMd2* texCoords = calloc(1, mdl->header.numOfSt*sizeof(STexCoordMd2));
        if (texCoords == NULL)
            goto result;

        // The texture coordinates in model
        //mdl->texCoords = calloc(1, mdl->header.numOfVertices*sizeof(SVector2f));
        mdl->texCoords = calloc(1, 3*mdl->header.numOfTris*sizeof(SVector2f));
        if (mdl->texCoords == NULL)
            goto result;

        memcpy(texCoords, buffer + mdl->header.offsetSt, mdl->header.numOfSt*sizeof(STexCoordMd2));

        // unpack texture coordinates from file to full buffer
        // делаем предрасчет текстурных координат
        uint32     arrL = 0;   // длина буффера
        float      s, t;

        for (int32 i = 0; i < mdl->header.numOfTris; ++i)
        {
            for (int32 j = 0; j < 3; ++j)
            {
                s = (float)(texCoords[mdl->triangles[i].st[j]].s) / mdl->header.skinWidth;
                t = (float)(texCoords[mdl->triangles[i].st[j]].t) / mdl->header.skinHeight;

                mdl->texCoords[arrL] = NewVector2f(s, t);
                ++arrL;
            }
        }

        free (texCoords);
        texCoords = NULL;
    }

    isLoaded = true;

result:

    if (mesh->model != NULL)
    {
        free(mesh->model);
        mesh->model = NULL;
    }

    /* set model info to our mesh */
    if (!isLoaded)
    {
        free(mdl);
        mdl = NULL;
    }
    else
        mesh->model = mdl;

    free(buffer);
    buffer = NULL;

    return isLoaded;
}


bool RM_LoadMeshBsp(const char* fileName, void** meshBsp)
{
    if (!FileExists(fileName))
    {
        LogWriteError("File '%s' doesn't exist!\n", fileName);
        return false;
    }

    /*
    if (meshBsp == NULL || !ObjectIsMeshBsp(*meshBsp))
        return false;

    */

    SMeshBsp* mesh = (SMeshBsp*)*meshBsp;
    uint8* buffer = NULL;
    int32  bufSize = FileLoad(fileName, (void**)(&buffer));
    bool isLoaded = false;
    if (bufSize <= 0)
    {
        LogWriteError("File '%s' is not loaded!\n", fileName);
        return false;
    }

    uint32 len;

    SHeaderBsp header = {0};

    /*
    SVertexBsp* vertices = NULL;
    uint32 numOfVerts = 0;

    SEdgeBsp* edges = NULL;
    //int32 numOfEdges = 0;

    SFaceBsp* faces = NULL;
    //int32 numOfFaces = 0;

    SLeafBsp* leaves = NULL;
    //int32 numOfLeaves = 0;

    SNodeMapBsp* nodes = NULL;
    //int32 numOfNodes = 0;

    SPlaneBsp* planes = NULL;
    //int32 numOfPlanes = 0;

    STexInfoBsp* texInfo = NULL;
    uint32 numOfTexInfo = 0;

    uint8* visInfo = NULL;

    uint32* faceEdgeTable = NULL;

    uint16* leafFaceTable = NULL;
    */

	// Read the header data
	memcpy(&header, buffer, sizeof(SHeaderBsp));

	// .bsp files MUST have a version of 38 and magic word IBSP
	if (header.version != 38 ||
        header.ident   != 0x50534249)     // 'I' 'B' 'S' 'P' = 0x50534249
	{
		// Display an error message for bad file format, then stop loading
		LogWriteError("Invalid file format: %s!\n", fileName);
		goto result;
	}

    // читаем вершины
    // read vertices
    len = header.lump[BSP_VERTICES].length;
    mesh->numOfVerts = len / sizeof(SVector3f);
    free(mesh->vertices);
    mesh->vertices = calloc(1, len);
    memcpy(mesh->vertices, buffer + header.lump[BSP_VERTICES].offset, len);
    // переворачиваем оси
    for (uint32 i = 0; i < mesh->numOfVerts; ++i)
    {
        Swap(&(mesh->vertices[i].y), &(mesh->vertices[i].z));
        mesh->vertices[i].z = -mesh->vertices[i].z;
    }

    // read edges
    len = header.lump[BSP_EDGES].length;
    mesh->numOfEdges = len / sizeof(SEdgeBsp);
    free(mesh->edges);
    mesh->edges = calloc(1, len);
    memcpy(mesh->edges, buffer + header.lump[BSP_EDGES].offset, len);

    // read faces
    len = header.lump[BSP_FACES].length;
    mesh->numOfFaces = len / sizeof(SFaceBsp);
    free(mesh->faces);
    mesh->faces = calloc(1, len);
    memcpy(mesh->faces, buffer + header.lump[BSP_FACES].offset, len);

    // read leaves
    len = header.lump[BSP_LEAVES].length;
    mesh->numOfLeaves = len / sizeof(SLeafBsp);
    free(mesh->leaves);
    mesh->leaves = calloc(1, len);
    memcpy(mesh->leaves, buffer + header.lump[BSP_LEAVES].offset, len);

    // read nodes
    len = header.lump[BSP_NODES].length;
    mesh->numOfNodes = len / sizeof(SNodeMapBsp);
    free(mesh->nodes);
    mesh->nodes = calloc(1, len);
    memcpy(mesh->nodes, buffer + header.lump[BSP_NODES].offset, len);

    // read planes
    len = header.lump[BSP_PLANES].length;
    mesh->numOfPlanes = len / sizeof(SPlaneBsp);
    free(mesh->planes);
    mesh->planes = calloc(1, len);
    memcpy(mesh->planes, buffer + header.lump[BSP_PLANES].offset, len);

    // read texInfo
    len = header.lump[BSP_TEXINFO].length;
    mesh->numOfTexInfo = len / sizeof(STexInfoBsp);
    free(mesh->texInfo);
    mesh->texInfo = calloc(1, len);
    memcpy(mesh->texInfo, buffer + header.lump[BSP_TEXINFO].offset, len);
    //for (uint32 i = 0; i < mesh->numOfTexInfo; ++i)
    //    printf("texture name = %s\n", mesh->texInfo[i].textureName);

    // read visInfo
    len = header.lump[BSP_VISIBILITY].length;
    free(mesh->visInfo);
    mesh->visInfo = calloc(1, len);
    memcpy(mesh->visInfo, buffer + header.lump[BSP_VISIBILITY].offset, len);

    // read face edge table
    len = header.lump[BSP_FACE_EDGE_TABLE].length;
    free(mesh->faceEdgeTable);
    mesh->faceEdgeTable = malloc(len);
    memcpy(mesh->faceEdgeTable, buffer + header.lump[BSP_FACE_EDGE_TABLE].offset, len);

    // read leaf face table
    len = header.lump[BSP_LEAF_FACE_TABLE].length;
    free(mesh->leafFaceTable);
    mesh->leafFaceTable = malloc(len);
    memcpy(mesh->leafFaceTable, buffer + header.lump[BSP_LEAF_FACE_TABLE].offset, len);



result :

    free(buffer);
    buffer = NULL;

    /*
    free(vertices);
    free(edges);
    free(faces);
    free(leaves);
    free(nodes);
    free(planes);
    free(texInfo);
    free(visInfo);
    free(faceEdgeTable);
    free(leafFaceTable);
    */

    return true;
}
