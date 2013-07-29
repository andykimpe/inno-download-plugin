#include <tchar.h>
void debugprintf(const _TCHAR *format, ...);

#ifdef _DEBUG
#define TRACE(f, ...) debugprintf(f, __VA_ARGS__)
#else
#define TRACE(f, ...)
#endif