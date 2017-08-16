#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "log_system.h"

static FILE* _outputFile;
static char _outputFileName[256] = "";
static bool _writeToOutput = false;

static void LogWrite(const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    //perror(buffer);
    va_end(args);

    printf("%s", buffer);
    if(_writeToOutput && _outputFile)
    {
        fprintf(_outputFile, "%s", buffer);
        fflush(_outputFile);
    }
}

void LogWriteMessage(const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    //perror(buffer);
    va_end(args);

    LogWrite(buffer);
}
void LogWriteError(const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    //perror(buffer);
    va_end(args);

    LogWrite("ERROR! %s", buffer);
}
void LogWriteWarning(const char* format, ...)
{
    //printf("Warning! %s", message);
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    //perror(buffer);
    va_end(args);

    LogWrite("Warning! %s", buffer);
}

void LogSetOutputFile(const char* fileName)
{
    if(fileName)
    {
        memset(_outputFileName, 0, sizeof(_outputFileName));

        strcpy(_outputFileName, fileName);
    }
}

void LogSetWritingToOutputFile(bool writeToOutput)
{
    if(writeToOutput)
    {
        if(strlen(_outputFileName) == 0)
        {
            LogWriteError("Output file not specified!\n");
            return;
        }
        else
        {
            _outputFile = fopen(_outputFileName, "w");
            assert(_outputFile);

            /* print starting time to log */
            char buffer[30] = "";
            time_t now = time(NULL);
            strftime(buffer, 29, "%Y-%m-%d %H:%M:%S", localtime(&now));
            fprintf(_outputFile, "...start logging at %s...\n\n", buffer);
            fflush(_outputFile);
        }
    }
    else
    {
        fclose(_outputFile);
        _outputFile = NULL;
    }

    _writeToOutput = writeToOutput;
}
