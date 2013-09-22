function findNotes(n)
	local r = n:gsub("{note%-%d}", function(s)
		local num = s:match("%d")
		return '<sup><a href="#note-' .. num .. '">' .. num .. '</a></sup>'
	end)
	return r
end

function parseProto(proto)
	local r, n = proto:gsub("%s%a+%(", function(s)
		local name = s:match("%a+")
		return " <b>" .. name .. "</b>("
	end)
	if n == 0 then
		r = proto:gsub("%s%a+:", function(s)
			local name = s:match("%a+")
			return " <b>" .. name .. "</b>:"
		end)
	end
	return r
end 

outfile = io.stdout

function prn(...)
	local args = {...}
	for k, v in pairs(args) do
		outfile:write(findNotes(v))
	end
end

function sortedpairs(t)
	local keys = {}
	local i = 1
	for key, val in pairs(t) do
		keys[i] = key
		i = i + 1
	end
	table.sort(keys)
	return coroutine.wrap(function()
		for i, key in ipairs(keys) do
			coroutine.yield(key, t[key])
		end
	end)
end

function htmlheader(title)
	prn([[
<html>
<head>
  <title>]], title, [[</title>
  <link rel="stylesheet" type="text/css" href="styles.css"/>
</head>
<body>
]])
end

function setout(filename)
	outfile = io.open(filename, "w")
end

function writePage(page, title)
	setout((page.title or title) .. ".htm")
	htmlheader(page.title or title)

	prn("<pre class=\"proto\">", parseProto(page.proto), "</pre>\n")
	prn("<p>", page.desc or "", "</p>\n<dl>\n")
	
	if page.params ~= nil then
		prn("<dt>Parameters:</dt><dd><p><table>\n");
		for i, param in ipairs(page.params) do
			if i == 1 then
				prn("  <tr><td><tt>", param[1], "</tt></td><td class=\"wide\">", param[2], "</td></tr>\n")
			else
				prn("  <tr><td><tt>", param[1], "</tt></td><td>", param[2], "</td></tr>\n")
			end
		end
		prn("</table></p></dd>\n")
	end
	
	if page.options ~= nil then
		prn("<dt>Options:</dt><dd><p><table>\n");
		prn("  <tr><th>Name</th><th class=\"wide\">Description</th><th>Default</th></tr>\n")
		for i, option in ipairs(page.options) do
			prn("  <tr><td><tt>", option[1], "</tt></td><td>", option[2], "</td><td><tt>", option[3],"</tt></td></tr>\n")
		end
		prn("</table></p></dd>\n")
	end
	
	if page.returns ~= nil then
		prn("<dt>Returns:</dt><dd>\n")
		prn("  <p>", page.returns, "</p>\n")
		prn("</dd>\n")
	end;
	
	if page.example ~= nil then
		prn("<dt>Example:</dt><dd>\n")
		prn("<pre>", page.example, "</pre>\n")
		prn("</dd>\n")
	end;

	if page.notes ~= nil then
		prn("<dt>Notes:</dt><dd><p>\n")
		for i, note in ipairs(page.notes) do
			prn("  <a id=\"note-" .. i .. "\"><sup>" .. i .. "</sup></a>", note, "<br/>\n")
		end
		prn("</p></dd>\n")
	end
	
	if page.seealso ~= nil then
		prn("<dt>See also:</dt><dd><p>\n")
		for i, sa in ipairs(page.seealso) do
			prn([[  <a href="]], _G[sa].title or sa, [[.htm">]], sa, "</a><br/>\n")
		end
		prn("</p></dd>\n")
	end

	prn "</dl>\n</body>\n</html>\n"
	outfile:close()
end

function writePages(ref)
	io.write "Generating reference...\n"
	for title, page in sortedpairs(ref) do
		io.write("    ", title, "\n")
		writePage(page, title)
	end
end

function buildReference()
	local t = {}
	local i = 1
	for k, v in pairs(_G) do
		if k:sub(1, 3) == "idp" then
			t[k] = v
			i = i + 1
		end
	end
	
	return t
end

function writeRefPage(ref)
	io.write "Generating HTML contents...\n"
	setout "Reference.htm"
	htmlheader "Reference"
	prn[[
<h3>Function list</h3>
<ul>
]]
	for title, page in sortedpairs(ref) do
		prn('  <li><a href="', (page.title or title), '.htm">', title, "</a></li>\n")
	end
	prn[[
</ul>
</body>
</html>
]]
	outfile:close()
end

function writeTOC(ref)
	io.write "Generating HTMLHelp contents...\n"
	setout "Contents.hhc"
	prn[[
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<HTML>
<HEAD>
<!-- Sitemap 1.0 -->
</HEAD><BODY>
<UL>
	<LI> <OBJECT type="text/sitemap">
		<param name="Name" value="Reference">
		<param name="Local" value="Reference.htm">
		</OBJECT>
	<UL>
]]
	for title, page in sortedpairs(ref) do
		prn([[
		<LI> <OBJECT type="text/sitemap">
			<param name="Name" value="]], title, [[">
			<param name="Local" value="]], (page.title or title), [[.htm">
			</OBJECT>
]])
	end
	prn[[
	</UL>
</UL>
</BODY></HTML>
]]
	outfile:close()
end

function buildIndex(ref)
	local idx = {}
	for title, page in pairs(ref) do
		idx[title] = (page.title or title)
		
		if page.options ~= nil then
			for i, option in ipairs(page.options) do
				idx[option[1]] = (page.title or title)
			end
		end
		
		if page.keywords ~= nil then
			for i, keyword in pairs(page.keywords) do
				idx[keyword] = (page.title or title)
			end
		end
	end
	
	return idx
end

function writeHHK(idx)
	io.write "Generating HTMLHelp index...\n"
	setout "Index.hhk"
	prn[[
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<HTML>
<HEAD>
<!-- Sitemap 1.0 -->
</HEAD><BODY>
<UL>
]]
	for key, page in pairs(idx) do
		prn([[
	<LI> <OBJECT type="text/sitemap">
		<param name="Name" value="]],  key, [[">
		<param name="Name" value="]],  page, [[">
		<param name="Local" value="]], page, [[.htm">
		</OBJECT>
]])
	end
	
	prn[[
</UL>
</BODY></HTML>
]]
end

function writeHHP(ref)
	io.write "Generating HTMLHelp project file...\n"
	setout "idp.hhp"
	prn[[
[OPTIONS]
Compatibility=1.1 or later
Compiled file=idp.chm
Contents file=Contents.hhc
Default Window=main
Default topic=Reference.htm
Display compile progress=Yes
Full-text search=Yes
Index file=Index.hhk
Language=0x409 Английский (США)
Title=Inno Download Plugin

[WINDOWS]
main=,"Contents.hhc","Index.hhk","Reference.htm","Reference.htm",,,,,0x42520,,0x10304e,[88,80,869,673],,,,,,,0

[FILES]
Reference.htm
]]
	for title, page in sortedpairs(ref) do
		prn((page.title or title), ".htm\n")
	end
	
	outfile:close();
end
