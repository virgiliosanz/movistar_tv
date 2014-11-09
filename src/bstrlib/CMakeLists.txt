file(GLOB_RECURSE GENERIC_SOURCES ./ *.c *.h)
add_library(bstrlib ${GENERIC_SOURCES})
