#pragma once

#include <windows.h>
#include <map>
#include "tstring.h"

class ErrorDialog
{
public:
	ErrorDialog();
	~ErrorDialog();

protected:
	HWND listBox;
};
