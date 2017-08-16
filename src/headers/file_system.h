/*
	It's part of bodhi GameEngine.
    Copyright (C) 2017 Anton "Vuvk" Shcherbatykh

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "types.h"

#define MAX_FILE_EXT_LENGTH 10

/** return true (1) if file exists */
bool FileExists(const char* fileName);
/** return true if no error. write extension in upper */
bool FileExtractExt(char** extension, const char* fileName);

/** return size of opened file in bytes */
uint32 FileLength (FILE* f);
/** return size of not opened file in bytes */
uint32 FileLengthByName (const char* fileName);
/** load file from path if it exists, try to load file from pak if not. Return size in bytes. Return -1 if error */
int32 FileLoad(const char* path, void** buffer);


/** free loaded buffer */
//void FileFree (void* buffer);
//int32 FileFOpen (const char *fileName, FILE **file);
/** read len bytes from opened file to buffer with tries */
//int32 FileRead(void* buffer, int32 len, FILE* f, uint8 maxTries);
/** read len bytes from opened file to buffer with tries */

