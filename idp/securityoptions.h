#pragma once

#include "tstring.h"

#define INVC_SHOWDLG 0
#define INVC_STOP    1
#define INVC_IGNORE  2

class SecurityOptions
{
public:
	SecurityOptions(tstring lgn = _T(""), tstring pass = _T(""), int invAct = INVC_SHOWDLG);
	~SecurityOptions();

	bool hasLoginInfo();

	tstring	login;
	tstring	password;
	int     invalidCertAction;
};
