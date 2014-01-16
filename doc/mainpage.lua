mainpage = [[
<h2>Inno Download Plugin</h2>
<img src="screenshot.png"/>
<p>Inno Download Plugin is a plugin for Inno Setup that allows you to download files from the Internet during the installation process. It supports FTP, HTTP and HTTPS protocols.</p>
<a id="installation"><h3>Installation</h3></a>
<dl>
<dt>Installation from self-extracting installer</dt><dd><p>
Run the installer. It will install IDP and add IDP include path to <b>ISPPBuiltins.iss</b>.
</p></dd>
<dt>Installation from zip archive</dt><dd><p>
Extract all files to any directory you wish. Add following line to the end of <b>%ProgramFiles%\Inno Setup 5\ISPPBuiltins.iss</b> file (optional):
<pre>#pragma include __INCLUDE__ + ";" + "c:\Full\Path\To\InnoDownloadPlugin"</pre>
</p></dd>
</dl>
<a id="usage"><h3>Usage</h3></a>
To add download functionality to your installation script:
<ul>
  <li>Include IDP include file: <code>#include &lt;idp.iss&gt;</code></li>
  <li>For languages other than English, include one or more language files: <code>#include &lt;idplang\Russian.iss&gt;</code></li>
  <li>Call <a href="idpAddFile, idpAddFileSize.htm">idpAddFile</a> to add files for downloading</li>
  <li>Call <a href="idpDownloadAfter.htm">idpDownloadAfter</a></li>
</ul>
Example:
<pre>procedure <b>InitializeWizard</b>();
begin
  idpAddFile('http://www.example.com/test1.zip', ExpandConstant('{tmp}\test1.zip'));
  idpAddFile('http://www.example.com/test2.zip', ExpandConstant('{tmp}\test2.zip'));
  idpAddFile('http://www.example.com/test3.zip', ExpandConstant('{tmp}\test3.zip'));

  idpDownloadAfter(wpReady);
end;

procedure <b>CurStepChanged</b>(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then 
  begin
    FileCopy(ExpandConstant('{tmp}\test1.zip'), ExpandConstant('{app}\test1.zip'), false);
    FileCopy(ExpandConstant('{tmp}\test2.zip'), ExpandConstant('{app}\test2.zip'), false);
    FileCopy(ExpandConstant('{tmp}\test3.zip'), ExpandConstant('{app}\test3.zip'), false);
  end;
end;</pre>
For more examples, see <b>examples</b> folder.
<a id="links"><h3>Links &amp; Copyright</h3></a>
For updates and support please visit:
<ul>
  <li><a href="http://mitrich.net23.net/">Website</a></li>
  <li><a href="http://groups.google.com/group/inno-download-plugin">Discussion forum</a></li>
  <li><a href="https://code.google.com/p/inno-download-plugin/">Source code repository</a></li>
</ul>
Inno Download Plugin &copy;2013-]] .. os.date("%Y") .. [[ Mitrich Software
<p>
Translations provided by:
<table>
  <tr><td>Belarusian</td><td>pavlushko.m</td></tr>
  <tr><td>German</td>    <td>fois</td></tr>
  <tr><td>Polish</td>    <td>Adam Siwon</td></tr>
</table>
</p>
]]
