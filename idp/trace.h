#pragma once

#include <tchar.h>
void debugprintf(const _TCHAR *format, ...);

#ifdef _DEBUG
#define TRACE(fmt, ...) debugprintf(fmt, ##__VA_ARGS__)
#else
#define TRACE(fmt, ...)
#endif
