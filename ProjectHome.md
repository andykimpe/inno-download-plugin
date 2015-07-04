# IDP: Download plugin for Inno Setup. #

![http://imageshack.com/a/img545/638/prcb.png](http://imageshack.com/a/img545/638/prcb.png)

### Features: ###
  * Supports **Unicode** & **ANSI** Inno Setup versions
  * **FTP**, **HTTP** and **HTTPS** protocols
  * Multiple languages
  * Free and open source under [Zlib license](http://opensource.org/licenses/Zlib)

### Supported languages: ###
  * Belarusian
  * Brazilian Portuguese
  * English
  * Finnish
  * German
  * Italian
  * Polish
  * Russian
  * Simplified Chinese

### Basic example: ###
```
#include <idp.iss>

[Files]
Source: "{tmp}\file1.xyz"; DestDir: "{app}"; Flags: external; ExternalSize: 1048576
Source: "{tmp}\file2.xyz"; DestDir: "{app}"; Flags: external; ExternalSize: 1048576
Source: "{tmp}\file3.xyz"; DestDir: "{app}"; Flags: external; ExternalSize: 1048576

[Icons]
Name: "{group}\{cm:UninstallProgram,My Program}"; Filename: "{uninstallexe}"

[Code]
procedure InitializeWizard();
begin
    idpAddFileSize('http://127.0.0.1/file1.xyz', ExpandConstant('{tmp}\file1.xyz'), 1048576);
    idpAddFileSize('http://127.0.0.1/file2.xyz', ExpandConstant('{tmp}\file2.xyz'), 1048576);
    idpAddFileSize('http://127.0.0.1/file3.xyz', ExpandConstant('{tmp}\file3.xyz'), 1048576);

    idpDownloadAfter(wpReady);
end;
```
[More examples...](https://code.google.com/p/inno-download-plugin/source/browse/examples)

### Download: ###
Latest version is 1.5.0 (14 Jan 2015)
  * [Download version 1.5.0 (Yandex Disk)](https://yadi.sk/d/B1GXlnwXdxggz)
  * [Download other releases (Yandex Disk)](https://yadi.sk/d/y1tTqndxVf7Uh)
  * [Download other releases (Google Drive)](https://drive.google.com/folderview?id=0Bzw1xBVt0mokSXZrUEFIanV4azA&usp=sharing#list)

### Discussions & support: ###
  * [Inno Download Plugin on Google Groups](https://groups.google.com/forum/#!forum/inno-download-plugin)