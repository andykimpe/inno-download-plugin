#pragma once

#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include "tstring.h"

class Url
{
public:
	Url(tstring address);
	~Url();

	HINTERNET connect(HINTERNET internet);
	HINTERNET open(HINTERNET internet, _TCHAR *httpVerb = NULL);
	void	  disconnect();
	void      close();
	DWORDLONG getSize(HINTERNET internet);

protected:
	tstring        url;
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
