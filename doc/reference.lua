idpAddFile = {
	proto = [[
procedure idpAddFile(url, filename: String);
procedure idpAddFileSize(url, filename: String; size: Int64{note-1});
]],
	title = "idpAddFile, idpAddFileSize",
	desc  = "Adds file to download list. User name, password and port number can be specified as part of the URL.",
	params = {
		{ "url",      "Full file URL" },
		{ "filename", "File name on the local disk." },
		{ "size",     "Size of file." }
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
	proto   = "procedure idpClearFiles;",
	desc    = "Clear all files, previously added with idpAddFile() procedure",
	seealso = { "idpAddFile" }
}

idpFilesCount = {
	proto   = "function idpFilesCount: Integer;",
	desc    = "Returns number of files, previously added with idpAddFile() procedure.",
	returns = "Number of files",
	seealso = { "idpAddFile", "idpClearFiles" }
}

idpFilesDownloaded = {
	proto =   [[function idpFilesDownloaded: Boolean;]],
	desc  =   [[Returns download status. If <tt>AllowContinue</tt> option was set to <tt>1</tt>, this function can be
				used to check that all files was successfully downloaded. If at least one file wasn't downloaded, 
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
	proto = "function idpDownloadFile(url, filename: String): Boolean; ",
	desc  = "Immediately download given file. Returns when file downloaded.",
	params = {	
		{ "url",      "Full file URL." },
		{ "filename", "File name on the local disk." }
	},
	returns = "<tt>True</tt> if file was successfully downloaded, <tt>False</tt> otherwise",
	seealso = { "idpDownloadFiles" }
}

idpDownloadFiles = {
	proto   = "function idpDownloadFiles: Boolean;",
	desc    = "Immediately download all files, previously added with idpAddFile() procedure. Returns when all files downloaded.",
	returns = idpFilesDownloaded.returns,
	seealso = { "idpDownloadFile", "idpDownloadAfter" }
}

idpDownloadAfter = {
	proto = "procedure idpDownloadAfter(pageAfterId: Integer);",
	desc  = "Inform IDP that download should be started after given page.",
	params = {
		{ "pageAfterID", "Wizard page ID" }
	},
	example = idpAddFile.example,
	seealso = { "idpAddFile" }
}

idpGetFileSize = {
	proto  = "function idpGetFileSize(url: String; var size: Int64{note-1}): Boolean;",
	desc   = "Gets size of file at given URL.",
	params = {
		{ "url",  "File url" },
		{ "size", "The variable to store the size into" }
	},
	returns = "<tt>True</tt> if operation was successfull, <tt>False</tt> otherwise",
	notes   = { "<tt>size</tt> parameter is <tt>Dword</tt> for ANSI Inno Setup" },
	seealso = { "idpGetFilesSize" },
	example = [[
var size: Int64;
...
if idpGetFileSize('http://www.example.com/file.zip', size) then
  // Do something useful with file size...
]]
}

idpGetFilesSize = {
	proto = "function idpGetFilesSize(var size: Int64{note-1}): Boolean;",
	desc  = "Get size of all files, previously added with idpAddFile() procedure.",
	params = {
		{ "size", "The variable to store the size into" }
	},
	returns = idpGetFileSize.returns,
	notes   = idpGetFileSize.notes,
	seealso = { "idpGetFileSize" }
}

idpSetOption = {
	proto = "procedure idpSetOption(name, value: String);",
	desc  = "Set value of IDP option. Option name is case-insensitive.",
	params = {
		{ "name",  "Option to set" },
		{ "value", "Option value as string" }
	},
	options = {
		{ "AllowContinue",     [[Allow user to continue installation if download fails. If set to <tt>1</tt>,
		                         you can use idpFilesDownloaded function to check download status]],           "0" },
		{ "DetailsVisible",    "If set to <tt>1</tt>, download details will be visible by default",            "0" },
		{ "DetailsButton",     "Controls availability of 'Details' button",                                    "1" },
		{ "RetryButton",       [[Controls availability of 'Retry' button on wizard form. If set to <tt>0</tt>,
		                         'Download failed' message box will have 'Retry' & 'Cancel' buttons]],         "1" },
		{ "UserAgent",         "User Agent string, used in HTTP and HTTPS requests",                           "InnoDownloadPlugin/1.0" },
		{ "InvalidCertAction", [[Action to perform, when HTTPS certificate is invalid. Possible values are:
		                         <ul>
		                         <li><tt>ShowDlg</tt> &ndash; Show error dialog, allowing user to view
								                      certificate details, cancel download or ignore error</li>
								 <li><tt>Ignore</tt>  &ndash; Ignore error and continue download</li>
								 <li><tt>Stop</tt>    &ndash; Stop download</li>
								 </ul>]],                                                                      "ShowDlg" },
	},
	keywords = {"user agent", "ShowDlg", "Ignore", "Stop"},
	example = [[
idpSetOption('AllowContinue',     '1');
idpSetOption('DetailsVisible',    '1');
idpSetOption('DetailsButton',     '0');
idpSetOption('RetryButton',       '0');
idpSetOption('UserAgent',         'Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/29.0.1547.66 Safari/537.36');
idpSetOption('InvalidCertAction', 'ignore');
]]
}
