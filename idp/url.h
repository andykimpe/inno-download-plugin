#pragma once

#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include "tstring.h"
#include "securityoptions.h"

#define FILE_SIZE_UNKNOWN 0xffffffffffffffffULL

class Url
{
public:
	Url(tstring address);
	~Url();

	HINTERNET connect(HINTERNET internet);
	HINTERNET open(HINTERNET internet, const _TCHAR *httpVerb = NULL);
	void	  disconnect();
	void      close();
	DWORDLONG getSize(HINTERNET internet);
	void      setSecurityOptions(SecurityOptions opt);

	tstring         urlString;
	SecurityOptions securityOptions;

protected:
	URL_COMPONENTS urlComponents;
	_TCHAR        *scheme;
	_TCHAR        *hostName;
	_TCHAR        *userName;
	_TCHAR        *password;
	_TCHAR        *urlPath;
	_TCHAR        *extraInfo;
	DWORD          service;
	HINTERNET      connection;
	HINTERNET      filehandle;
};
