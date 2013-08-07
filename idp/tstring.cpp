#include <windows.h>
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

tstring itotstr(int d)
{
	_TCHAR buf[34];
	_itot(d, buf, 10);
	return buf;
}
