#pragma once

#include <string>
#include <tchar.h>

using namespace std;

typedef basic_string<_TCHAR> tstring;

string  toansi(tstring s);
tstring tocurenc(string s);
tstring tstrlower(_TCHAR *s);
tstring tstrprintf(tstring format, ...);
tstring itotstr(int d);
string  dwtostr(unsigned long d);
tstring formatsize(unsigned long long size, tstring kb, tstring mb, tstring gb);
tstring formatsize(tstring ofmsg, unsigned long long size1, unsigned long long size2, tstring kb, tstring mb, tstring gb);
tstring formatspeed(unsigned long speed, tstring kbs, tstring mbs);
