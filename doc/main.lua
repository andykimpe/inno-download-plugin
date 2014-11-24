groups = {}
reference = {
    group = function(title)
        groups[title] = {}
        
        setmetatable(reference, {
            __newindex = function(t, k, v)
                rawset(reference, k, v)
                groups[title][k] = v
            end})
    end
}

buildRef = loadfile("reference.lua")
setfenv(buildRef, reference);

buildRef()
reference.group = nil

--dofile "reference.lua"
dofile "mainpage.lua"
dofile "history.lua"
dofile "generator.lua"

--reference = buildReference()
index     = buildIndex(reference)

writePages(reference)
writeRefPage(reference)
writeHtmlTOC(reference)
writeTOC(reference)
writeHHP(reference)
writeHHK(index)
writeMainPage(mainpage)
writeHistory(history)
writeLicense("../COPYING.txt")