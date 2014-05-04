#pragma once

#include <windows.h>
#include <map>
#include "netfile.h"
#include "tstring.h"

#define DLG_NONE     0
#define DLG_SIMPLE   1
#define DLG_FILELIST 2
#define DLG_URLLIST  3

using namespace std;

class Ui;

class ErrorDialog
{
public:
	ErrorDialog(Ui *parent = NULL);
	~ErrorDialog();

	void setUi(Ui *parent);
	void setFileList(map<tstring, NetFile *> fileList);
	int  exec();

protected:
	map<tstring, NetFile *> files;
	HWND                    listBox;
	Ui                     *ui;

	friend BOOL CALLBACK ErrorDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
};

BOOL CALLBACK ErrorDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
