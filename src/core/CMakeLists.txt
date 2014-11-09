file(GLOB_RECURSE GENERIC_SOURCES ./ *.c *.h)
add_library(core ${GENERIC_SOURCES})
