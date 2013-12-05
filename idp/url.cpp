#include <string.h>
#include "tstring.h"
#include "trace.h"
#include "url.h"
#include "ui.h"

Url::Url(tstring address)
{
	urlString = address;
	int len = (int)urlString.length();

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

	InternetCrackUrl(urlString.c_str(), 0, 0, &urlComponents);

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

	TRACE(_T("Connecting to %s://%s:%d..."), urlComponents.lpszScheme, hostName, urlComponents.nPort);
	connection = InternetConnect(internet, hostName, urlComponents.nPort, userName, password, service, flags, NULL);
	TRACE(_T("%s\n"), connection ? _T("Connected OK") : _T("Connection FAILED"));

	return connection;
}

HINTERNET Url::open(HINTERNET internet, const _TCHAR *httpVerb)
{
	LPCTSTR acceptTypes[] = { _T("*/*"), NULL };

	if(!connect(internet))
		return NULL;

	if(service == INTERNET_SERVICE_FTP)
		filehandle = FtpOpenFile(connection, urlPath, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);
	else
	{
		DWORD flags = INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION;

		if(urlComponents.nScheme == INTERNET_SCHEME_HTTPS)
		{
			flags |= INTERNET_FLAG_SECURE;

			if(internetOptions.invalidCert == INVC_IGNORE)
				flags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
		}

		tstring fullUrl = urlPath;
		fullUrl += extraInfo;
		TRACE(_T("Opening %s..."), fullUrl.c_str());
		filehandle = HttpOpenRequest(connection, httpVerb, fullUrl.c_str(), NULL, NULL, acceptTypes, flags, NULL);

retry:
		if(!HttpSendRequest(filehandle, NULL, 0, NULL, 0))
		{
			DWORD error = GetLastError();

			if((error == ERROR_INTERNET_INVALID_CA           ) ||
			   (error == ERROR_INTERNET_SEC_CERT_CN_INVALID  ) ||
			   (error == ERROR_INTERNET_SEC_CERT_DATE_INVALID))
			{
				TRACE(_T("Invalid certificate (0x%08x: %s)"), error, formatwinerror(error).c_str());

				if(internetOptions.invalidCert == INVC_SHOWDLG)
				{
					DWORD r = InternetErrorDlg(uiMainWindow(), filehandle, error,
						                       FLAGS_ERROR_UI_FILTER_FOR_ERRORS | FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS,
									           NULL);

					if((r == ERROR_SUCCESS) || (r == ERROR_INTERNET_FORCE_RETRY))
						goto retry;
					else if(r == ERROR_CANCELLED)
					{
						close();
						throw InvalidCertError("Download cancelled");
					}
				}
				else if(internetOptions.invalidCert == INVC_IGNORE)
				{
					DWORD flags;
					DWORD flagsSize = sizeof(flags);

					InternetQueryOption(filehandle, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&flags, &flagsSize);
					flags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
					InternetSetOption(filehandle, INTERNET_OPTION_SECURITY_FLAGS, &flags, sizeof(flags));

					goto retry;
				}
			}

			TRACE(_T("HttpSendRequest FAILED: 0x%08x - %s"), error, formatwinerror(error).c_str());
			return NULL;
		}

		DWORD dwStatusCode = 0, dwIndex = 0, dwBufSize;
		dwBufSize = sizeof(DWORD);

		if(!HttpQueryInfo(filehandle, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatusCode, &dwBufSize, &dwIndex))
		{
			TRACE(_T("HttpQueryInfo FAILED\n"));
			return NULL;
		}

		TRACE(_T("HTTP Status code: %d\n"), dwStatusCode);

		if((dwStatusCode != HTTP_STATUS_OK) && (dwStatusCode != HTTP_STATUS_CREATED/*Not sure, if this code can be returned*/))
		{
			close();
			throw HTTPError(dwtostr(dwStatusCode));
		}

		TRACE(_T("Request opened OK\n"));
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

	TRACE(_T("Getting size of %s...\n"), urlString.c_str());

	if(!open(internet, _T("HEAD")))
		return FILE_SIZE_UNKNOWN;

	if(service == INTERNET_SERVICE_FTP)
	{
		DWORD loword, hiword;
		loword = FtpGetFileSize(filehandle, &hiword);
		res = ((DWORDLONG)hiword << 32) | loword;
	}
	else
	{
		DWORD dwFileSize = 0, dwIndex = 0, dwBufSize;
		dwBufSize = sizeof(DWORD);

		if(!HttpQueryInfo(filehandle, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwFileSize, &dwBufSize, &dwIndex))
			return FILE_SIZE_UNKNOWN;

		res = dwFileSize;
	}

	TRACE(_T("Size of %s: %d bytes\n"), urlString.c_str(), (DWORD)res);
	close();

	return res;
}
