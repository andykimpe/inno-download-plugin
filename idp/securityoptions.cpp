#include "securityoptions.h"

SecurityOptions::SecurityOptions(tstring lgn, tstring pass, int invCert)
{
	login       = lgn;
	password    = pass;
	invalidCert = invCert;
}

SecurityOptions::~SecurityOptions()
{
}

bool SecurityOptions::hasLoginInfo()
{
	return (!login.empty()) || (!password.empty());
}
