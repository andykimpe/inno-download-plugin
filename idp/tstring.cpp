#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "tstring.h"

string toansi(tstring s)
{
#ifdef UNICODE
	int bufsize = (int)s.length()+1;
	char *buffer = new char[bufsize];
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), -1, buffer, bufsize, NULL, NULL);
	string res = buffer;
	delete[] buffer;
	return res;
#else
	return s;
#endif
}

tstring tocurenc(string s)
{
#ifdef UNICODE
	int bufsize = (int)s.length()+1;
	wchar_t *buffer = new wchar_t[bufsize];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, buffer, bufsize);
	tstring res = buffer;
	delete[] buffer;
	return res;
#else
	return s;
#endif
}

tstring itotstr(int d)
{
	_TCHAR buf[34];
	_itot(d, buf, 10);
	return buf;
}

string dwtostr(unsigned long d)
{
	char buf[34];
	_ultoa(d, buf, 10);
	return buf;
}

tstring tstrprintf(tstring format, ...)
{
	_TCHAR str[256];

	va_list argptr;
	va_start(argptr, format);
	_vstprintf(str, format.c_str(), argptr);
    va_end(argptr);

	return str;
}
