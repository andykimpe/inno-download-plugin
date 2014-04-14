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
		{ "size",     "Size of file (optional). If not specified, it will be determined when download begins." }
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

idpAddMirror = {
	proto   = "procedure idpAddMirror(url, mirror: String);",
	desc    = "Adds another URL for a given primary URL. The new URL will be used as a mirror if downloading from the original URL fails. You can add as many mirrors as you like",
	params  = {
		{ "url",    "Primary URL{note-1}" },
		{ "mirror", "Alternate URL" }
	},
	notes   = { "Unlike <tt>ITD_AddMirror</tt> procedure in <b>InnoTools Downloader</b>, mirrors are added for URLs, not for file names" },
	seealso = { "idpAddFile" }
}

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
	desc  = "Immediately download given file, without UI indication. Returns when file downloaded.",
	params = {	
		{ "url",      "Full file URL." },
		{ "filename", "File name on the local disk." }
	},
	returns = "<tt>True</tt> if file was successfully downloaded, <tt>False</tt> otherwise",
	seealso = { "idpDownloadFiles" }
}

idpDownloadFiles = {
	proto   = "function idpDownloadFiles: Boolean;",
	desc    = "Immediately download all files, previously added with idpAddFile() procedure, without UI indication. Returns when all files downloaded.",
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
    notes   = { 'When using <a href="http://www.graphical-installer.com/">Graphical Installer</a>, this function should be called <u>before</u> calling InitGraphicalInstaller()' },
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
	desc  = "Get size of all files, previously added with <a href=\"idpAddFile, idpAddFileSize.htm\">idpAddFile</a> procedure.",
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
		{ "AllowContinue",  [[Allow user to continue installation if download fails. If set to <tt>1</tt>,
		                      you can use <a href="idpFilesDownloaded.htm">idpFilesDownloaded</a> function 
							  to check download status]],                                                   "0{note-1}" },
		{ "DetailedMode",   "If set to <tt>1</tt>, download details will be visible by default",            "0" },
		{ "DetailsButton",  "Controls availability of 'Details' button",                                    "1" },
		{ "RetryButton",    [[Controls availability of 'Retry' button on wizard form. If set to <tt>0</tt>,
		                      'Download failed' message box will have 'Retry' & 'Cancel' buttons]],         "1" },
        { "RedrawBackground", "You may need to turn on this option when using background image for wizard pages{note-2}", "0" },
        { "SkinnedButton",  [[When using <a href="http://www.graphical-installer.com/">Graphical Installer</a>, 
                              turn on this option to get details button skinned]],                          "0" },
        { "Referer",        "Referer URL, to use in HTTP and HTTPS requests",                               ""  },
		{ "UserAgent",      "User Agent string, used in HTTP and HTTPS requests",                           "InnoDownloadPlugin/1.2" },
		{ "InvalidCert",    [[Action to perform, when HTTPS certificate is invalid. Possible values are:
		                         <ul>
		                         <li><tt>ShowDlg</tt> &ndash; Show error dialog, allowing user to view
								                      certificate details, cancel download or ignore error</li>
								 <li><tt>Ignore</tt>  &ndash; Ignore error and continue download</li>
								 <li><tt>Stop</tt>    &ndash; Stop download</li>
								 </ul>]],                                                                   "ShowDlg" },
		{ "ConnectTimeout", [[Time-out value, in milliseconds, to use for Internet connection requests.     
		                      Can be set to <tt>Infinite</tt> to disable this timer]],                      "</tt>System default{note-3}<tt>" },
		{ "SendTimeout",    "Time-out value, in milliseconds, to send a request",                           "</tt>System default<tt>" },
		{ "ReceiveTimeout", "Time-out value, in milliseconds, to receive a response to a request",          "</tt>System default<tt>" },
		
	},
	keywords = { "user agent", "timeout", "ShowDlg", "Ignore", "Stop"},
	notes    = { 
		"For boolean values, <tt>True/False</tt> and <tt>Yes/No</tt> also accepted",
        "When <tt>GRAPHICAL_INSTALLER_PROJECT</tt> is defined, <tt>RedrawBackground</tt> is turned on automatically",
		"Usually, 60 sec. for connect timeout and 30 sec. for send &amp; receive timeouts"
	},
	example  = [[
idpSetOption('AllowContinue',  '1');
idpSetOption('DetailedMode',   '1');
idpSetOption('DetailsButton',  '0');
idpSetOption('RetryButton',    '0');
idpSetOption('UserAgent',      'Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/29.0.1547.66 Safari/537.36');
idpSetOption('InvalidCert',    'ignore');
idpSetOption('ConnectTimeout', '10000');
]]
}
