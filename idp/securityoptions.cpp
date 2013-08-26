#include "securityoptions.h"

SecurityOptions::SecurityOptions(tstring lgn, tstring pass, int invAct)
{
	login			  = lgn;
	password		  = pass;
	invalidCertAction = invAct;
}

SecurityOptions::~SecurityOptions()
{
}

bool SecurityOptions::hasLoginInfo()
{
	return (!login.empty()) || (!password.empty());
}
