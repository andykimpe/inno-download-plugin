dofile "reference.lua"
dofile "generator.lua"

reference = buildReference()
index     = buildIndex(reference)

writePages(reference)
writeRefPage(reference)
writeTOC(reference)
writeHHP(reference)
writeHHK(index)
