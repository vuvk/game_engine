#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "types.h"
#include "utils.h"
#include "file_system.h"
#include "log_system.h"


bool FileExists(const char* fileName)
{
    FILE* file = fopen(fileName, "r");
    if (file != NULL)
    {
        fclose(file);
        file = NULL;
        return true;
    }

    LogWriteError("File '%s' doesn't exists!\n", fileName);
    return false;
}

bool FileExtractExt(char** extension, const char* fileName)
{
    if (extension == NULL || fileName == NULL)
        return false;

    char* dot = strrchr(fileName, '.');
    if (dot == NULL)
        return false;

    char ext[MAX_FILE_EXT_LENGTH] = "";
    int len;
    ++dot;
    for (len = 0; (*dot != '\0') && (len < MAX_FILE_EXT_LENGTH); ++len, ++dot)
        ext[len] = (char)toupper((int)*dot);

    if (len > MAX_FILE_EXT_LENGTH)
        len = MAX_FILE_EXT_LENGTH;

    free(*extension);
    *extension = calloc(1, len*sizeof(char));
    memcpy(*extension, ext, len*sizeof(char));

    return true;
}

/*
================
FileLength
================
*/
uint32 FileLength (FILE* f)
{
	int32   pos;
	int32   end;

	// запоминаем позицию, на которой стоял фалйовый поток
	pos = ftell (f);

	// прыгаем в конец - это и есть размер файла в байтах
	fseek (f, 0, SEEK_END);
	end = ftell (f);

	// возвращаемся туда, откуда начали
	fseek (f, pos, SEEK_SET);

	return end;
}

uint32 FileLengthByName (const char* fileName)
{
    if (!FileExists(fileName))
        return 0;

    FILE* file = fopen(fileName, "r");
	int32 len = FileLength(file);
	fclose(file);

	return len;
}

/*
===========
FileFOpen

Finds the file in the search path.
returns filesize and an open FILE*
===========
*/
int32 FileFOpen (const char* fileName, FILE** file)
{
    if (fileName == NULL || file == NULL)
        return -1;

	if (FileExists(fileName))
    {
        *file = fopen(fileName, "r");
        return FileLength(*file);
    }

	*file = NULL;
	return -1;
}

/*
=================
FileRead

Properly handles partial reads
Return loaded bytes count.
Return -1 if error
=================
*/
#define	MAX_READ	0x10000		// read in blocks of 64k
int32 FileRead(void* buffer, int32 len, FILE* f, uint8 maxTries)
{
    if (buffer == NULL || f == NULL)
        return -1;

	int32   block, remaining;
	int32   read;
	uint8*  buf;
	bool	tries;
	uint8   curTries;   // количество попыток уже совершенных

	buf = (uint8*)buffer;

	// read in chunks for progress bar
	remaining = len;
	tries = 0;
	curTries = 0;
	while (remaining)
	{
		block = remaining;
		if (block > MAX_READ)
			block = MAX_READ;
		read = fread (buf, 1, block, f);
		if (read == 0)
		{
			if (!tries)
			{
				tries = true;
			}
			else
            {
				LogWriteError("Read: 0 bytes read\n");
				// если прочтенных байт 0 штук и количество попыток чтения зашкаливает,
				// то вернуть число байт сколько смог прочитать
				if (++curTries >= maxTries)
                    return len - remaining;
            }
		}

		if (read == -1)
            LogWriteError("Read: -1 bytes read\n");

        //
        if (read > 0)
            curTries = 0;

		// do some progress bar thing here...

		remaining -= read;
		buf += read;
	}

	return len;
}

/*
==============
FileLoadFromPath
==============

load file from path to buffer

Return -1 if error, else return size of file in byte
*/
int32 FileLoadFromPath(const char* path, void** buffer)
{
	if (path == NULL || buffer == NULL)
        return -1;

	if (!FileExists(path))
        return -1;

	FILE* f = fopen(path, "rb");
	if (f == NULL)
        return -1;

	int32 len = FileLength(f);

	int8* buf = malloc(len*sizeof(int8));
	if (buf == NULL)
    {
        LogWriteError("Enough of memory!\n");
        return -1;
    }

	fseek(f, 0, SEEK_SET);
    int32 loadedBytes = FileRead (buf, len, f, 10);

    fclose(f);
    f = NULL;

    /* если удалось загрузить байт меньше, чем размер файла, то явно была ошибка! */
    if (loadedBytes != len)
    {
        LogWriteError("File not loaded!\n");
        free (buf);
        return -1;
    }

    *buffer = buf;
    buf = NULL;

	return len;
}
/*
==============
FileLoadFromPak
==============

load file from pak to buffer

Return -1 if error, else return size of file in byte
*/
int32 FileLoadFromPak(const char* path, void** buffer)
{
    if (path == NULL || buffer == NULL || *buffer == NULL)
        return -1;
    return -1;
}

/*
==============
FileLoad
==============

load file from path to buffer if it exists.
else try to load file from pak

Return -1 if error, else return size of file in byte
*/
int32 FileLoad(const char* path, void** buffer)
{
    int32 len = FileLoadFromPath(path, buffer);
    if (len == -1)
    {
        len = FileLoadFromPak(path, buffer);
    }

    return len;
}

/*
=============
FileFree
=============
*/
void FileFree (void* buffer)
{
    free(buffer);
}
