#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "utils.h"
#include "log_system.h"

static FILE* _outputFile;
static char _outputFileName[256] = "";
static bool _writeToOutput = false;

//static void LogWrite(const char* format, ...)
static void LogWrite(const char* message)
{
    if (StrLength(message) > 0)
    {
        printf("%s", message);
        if(_writeToOutput && _outputFile)
        {
            fprintf(_outputFile, "%s\n", message);
            fflush(_outputFile);
        }
    }
}

void LogWriteMessage(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // если передаваемых аргументов нет, то выход
    int32 lenArgs = _vscprintf(format, args);
    if (lenArgs < 1)
        return;

    char* buffer = NULL;

    // расчитываем длину принимающего буффера
    int32 lenBuffer = lenArgs + 1 /* '\0' */ ;
    buffer = calloc(lenBuffer, sizeof(char));

    vsprintf(buffer, format, args);
    //perror(buffer);
    va_end(args);

    LogWrite(buffer);

    free(buffer);
}

void LogWriteError(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // если передаваемых аргументов нет, то выход
    int32 lenArgs = _vscprintf(format, args);
    if (lenArgs < 1)
        return;

    char* buffer = NULL;

    // расчитываем длину принимающего буффера
    char msg[] = "ERROR! ";
    int32 lenMsg = StrLength(msg) + 1;
    int32 lenBuffer = lenArgs + lenMsg + 1 /* '\0' */ ;
    buffer = calloc(lenBuffer, sizeof(char));

    StrCopy(buffer, msg, lenMsg);

    vsprintf(buffer + lenMsg - 1, format, args);
    //perror(buffer);
    va_end(args);

    LogWrite(buffer);

    free(buffer);
}

void LogWriteWarning(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // если передаваемых аргументов нет, то выход
    int32 lenArgs = _vscprintf(format, args);
    if (lenArgs < 1)
        return;

    char* buffer = NULL;

    // расчитываем длину принимающего буффера
    char msg[] = "Warning! ";
    int32 lenMsg = StrLength(msg) + 1;
    int32 lenBuffer = lenArgs + lenMsg + 1 /* '\0' */ ;
    buffer = calloc(lenBuffer, sizeof(char));

    StrCopy(buffer, msg, lenMsg);

    vsprintf(buffer + lenMsg - 1, format, args);
    //perror(buffer);
    va_end(args);

    LogWrite(buffer);

    free(buffer);
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
