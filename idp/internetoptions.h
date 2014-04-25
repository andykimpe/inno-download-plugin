#pragma once

#include "tstring.h"

#define INVC_SHOWDLG 0
#define INVC_STOP    1
#define INVC_IGNORE  2

#define TIMEOUT_INFINITE 0xFFFFFFFF
#define TIMEOUT_DEFAULT  0xFFFFFFFE

#define IDP_USER_AGENT _T("InnoDownloadPlugin/1.2")

#define DWORD unsigned long

class InternetOptions
{
public:
	InternetOptions(tstring lgn = _T(""), tstring pass = _T(""), int invCert = INVC_SHOWDLG);
	~InternetOptions();

	bool hasLoginInfo();
	bool hasReferer();

	tstring	login;
	tstring	password;
	int     invalidCert;
	tstring referer;
	tstring userAgent;

	DWORD   connectTimeout;
	DWORD   sendTimeout;
	DWORD   receiveTimeout;
};
