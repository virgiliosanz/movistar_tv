file(GLOB_RECURSE GENERIC_SOURCES ./ *.c *.h)

add_library(generic ${GENERIC_SOURCES})

