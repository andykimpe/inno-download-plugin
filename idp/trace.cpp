#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

void debugprintf(const _TCHAR *format, ...)
{
	_TCHAR str[1024];

	va_list argptr;
    va_start(argptr, format);
    _vstprintf(str, format, argptr);
    va_end(argptr);

	OutputDebugString(str);
}