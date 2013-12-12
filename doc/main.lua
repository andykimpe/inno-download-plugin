dofile "reference.lua"
dofile "mainpage.lua"
dofile "history.lua"
dofile "generator.lua"

reference = buildReference()
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