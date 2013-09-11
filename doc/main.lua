dofile "reference.lua"
dofile "generator.lua"

reference = buildReference()
writePages(reference)
writeRefPage(reference)
writeTOC(reference)
writeHHP(reference)
