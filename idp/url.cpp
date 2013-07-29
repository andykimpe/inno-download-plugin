#include <string.h>
#include "tstring.h"
#include "trace.h"
#include "url.h"

Url::Url(tstring address)
{
	url = address;
	int len = (int)url.length();

	scheme    = new _TCHAR[len];
	hostName  = new _TCHAR[len];
	userName  = new _TCHAR[len];
	password  = new _TCHAR[len];
	urlPath   = new _TCHAR[len];
	extraInfo = new _TCHAR[len];

	urlComponents.dwStructSize      = sizeof(URL_COMPONENTS);
	urlComponents.lpszScheme        = scheme;
	urlComponents.dwSchemeLength    = len;
	urlComponents.lpszHostName      = hostName;
	urlComponents.dwHostNameLength  = len;
	urlComponents.lpszUserName      = userName;
	urlComponents.dwUserNameLength  = len;
	urlComponents.lpszPassword      = password;
	urlComponents.dwPasswordLength  = len;
	urlComponents.lpszUrlPath       = urlPath;
	urlComponents.dwUrlPathLength   = len;
	urlComponents.lpszExtraInfo     = extraInfo;
	urlComponents.dwExtraInfoLength = len;

	InternetCrackUrl(url.c_str(), 0, 0, &urlComponents);

	switch(urlComponents.nScheme)
	{
	case INTERNET_SCHEME_FTP  : service = INTERNET_SERVICE_FTP;  break;
	case INTERNET_SCHEME_HTTP : service = INTERNET_SERVICE_HTTP; break;
	case INTERNET_SCHEME_HTTPS: service = INTERNET_SERVICE_HTTP; break;
	}

	connection = NULL;
	filehandle = NULL;
}

Url::~Url()
{
	close();

	delete[] scheme;   
	delete[] hostName; 
	delete[] userName; 
	delete[] password; 
	delete[] urlPath;  
	delete[] extraInfo;
}

HINTERNET Url::connect(HINTERNET internet)
{
	DWORD flags = (service == INTERNET_SERVICE_FTP) ? INTERNET_FLAG_PASSIVE : 0;
	connection = InternetConnect(internet, hostName, urlComponents.nPort, userName, password, service, flags, NULL);

	return connection;
}

HINTERNET Url::open(HINTERNET internet)
{
	LPCTSTR acceptTypes[] = { _T("*/*"), NULL };

	connect(internet);

	if(service == INTERNET_SERVICE_FTP)
		filehandle = FtpOpenFile(connection, urlPath, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);
	else
	{
		filehandle = HttpOpenRequest(connection, NULL, urlPath, NULL, NULL, acceptTypes, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION, NULL);
		if(!HttpSendRequest(filehandle, NULL, 0, NULL, 0))
			return NULL;
	}

	return filehandle;
}

void Url::disconnect()
{
	if(connection)
		InternetCloseHandle(connection);

	connection = NULL;
}


void Url::close()
{
	if(filehandle)
		InternetCloseHandle(filehandle);

	filehandle = NULL;

	disconnect();
}

DWORDLONG Url::getSize(HINTERNET internet)
{
	DWORDLONG res;

	open(internet);

	if(service == INTERNET_SERVICE_FTP)
	{
		DWORD loword, hiword;
		loword = FtpGetFileSize(filehandle, &hiword);
		res = ((DWORDLONG)hiword << 32) | loword;
	}

	close();

	return res;
}
