idpAddFile = {
	proto = [[
procedure <b>idpAddFile</b>(url, filename: String);
procedure <b>idpAddFileSize</b>(url, filename: String; size: Int64{note-1});
]],
	title = "idpAddFile, idpAddFileSize",
	desc  = "Adds file to download queue.",
	params = {
		url      = "Url to file on server.",
		filename = "File name on the local disk.",
		size     = "Size of file."
	},
	notes    = { "<tt>size</tt> parameter is <tt>Dword</tt> for ANSI Inno Setup" },
	seealso  = { "idpClearFiles", "idpDownloadAfter", "idpDownloadFiles" },
	keywords = { "login", "password" },
	example = [[
procedure <b>InitializeWizard</b>();
begin
  idpAddFile('http://www.example.com/file1.dll', ExpandConstant('{tmp}\file1.dll'));
  idpAddFile('http://username:password@www.example.com/file2.dll', ExpandConstant('{tmp}\file2.dll'));

  idpDownloadAfter(wpReady);
end;
]]
}

idpAddFileSize = idpAddFile

idpClearFiles = {
	proto   = "procedure <b>idpClearFiles</b>;",
	desc    = "Clear all files, previously added with idpAddFile() procedure",
	seealso = { "idpAddFile" }
}

idpFilesCount = {
	proto   = "function <b>idpFilesCount</b>: Integer;",
	desc    = "Returns number of files, previously added with idpAddFile() procedure.",
	returns = "Nubmer of files",
	seealso = { "idpAddFile", "idpClearFiles" }
}

idpFilesDownloaded = {
	proto =   [[function <b>idpFilesDownloaded</b>: Boolean;]],
	desc  =   [[If <tt>AllowContinue</tt> option was set to <tt>1</tt>, this function can be used to check
				that all files was successfully downloaded. If at least one file wasn't downloaded, 
				this function returns <tt>False</tt>]],
	returns = [[<tt>True</tt> if all files was successfully downloaded, <tt>False</tt> otherwise]],
	example = [[
procedure <b>CurStepChanged</b>(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then 
    if idpFilesDownloaded then
    begin
      // Copy downloaded files to application directory
      Filecopy(ExpandConstant('{tmp}\file1.dll'), ExpandConstant('{app}\file1.dll'), false);
      Filecopy(ExpandConstant('{tmp}\file2.dll'), ExpandConstant('{app}\file2.dll'), false);
    end;
end;
]]
}

idpDownloadFile = {
	proto = "function <b>idpDownloadFile</b>(url, filename: String): Boolean; ",
	desc  = "Immediately download given file. Returns when file downloaded.",
	params = {	
		url      = "Url to file on server.",
		filename = "File name on the local disk."
	},
	returns = "<tt>True</tt> if file was successfully downloaded, <tt>False</tt> otherwise",
	seealso = { "idpDownloadFiles" }
}

idpDownloadFiles = {
	proto   = "function <b>idpDownloadFiles</b>: Boolean;",
	desc    = "Immediately download all files, previously added with idpAddFile() procedure. Returns when all files downloaded.",
	returns = idpFilesDownloaded.returns,
	seealso = { "idpDownloadFile", "idpDownloadAfter" }
}

idpDownloadAfter = {
	proto = "procedure <b>idpDownloadAfter</b>(PageAfterId: Integer);",
	desc  = "Inform IDP that download should be started after given page.",
	params = {
		pageAfterID = "Wizard page ID"
	},
	example = idpAddFile.example,
	seealso = { "idpAddFile" }
}

idpGetFileSize = {
	proto  = "function <b>idpGetFileSize</b>(url: String; var size: Int64{note-1}): Boolean;",
	desc   = "Get file size",
	params = {
		url = "File url",
		size = "The variable to store the size into"
	},
	returns = "<tt>True</tt> if operation was successfull, <tt>False</tt> otherwise",
	notes = { "<tt>size</tt> parameter is <tt>Dword</tt> for ANSI Inno Setup" },
	seealso = { "idpGetFilesSize" },
	example = [[
var size: Int64;
...
if idpGetFileSize('http://www.example.com/file.zip', size) then
  // Do something useful with file size...
]]
}

idpGetFilesSize = {
	proto = "function <b>idpGetFilesSize</b>(var size: Int64{note-1}): Boolean;",
	desc  = "Get size of all files, previously added with idpAddFile() procedure.",
	params = {
		size = "The variable to store the size into"
	},
	returns = idpGetFileSize.returns,
	notes   = idpGetFileSize.notes,
	seealso = { "idpGetFileSize" }
}

idpSetOption = {
	proto = "procedure <b>idpSetOption</b>(key, value: String);",
	desc  = "Set one of IDP options.",
	params = {
		key   = "Option to set",
		value = "Option value as string"
	},
	options = {
		AllowContinue     = { "Allow continue installation, if download fails", "0" },
		DetailsVisible    = { "", "0" },
		DetailsButton     = { "", "1" },
		RetryButton       = { "", "1" },
		UserAgent         = { "", "InnoDownloadPlugin/1.0" },
		InvalidCertAction = { "", "ShowDlg" },
	},
	keywords = {"user agent"},
	example = [[
idpSetOption('AllowContinue',     '1');
idpSetOption('DetailsVisible',    '1');
idpSetOption('DetailsButton',     '0');
idpSetOption('RetryButton',       '0');
idpSetOption('UserAgent',         'Godzilla Waterfox');
idpSetOption('InvalidCertAction', 'ignore');
]]
}
