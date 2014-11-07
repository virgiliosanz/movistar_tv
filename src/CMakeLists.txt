add_subdirectory(generic)
add_subdirectory(movistartv)

#--- executable
file(GLOB SRC_SOURCES ./ *.c *.h)
add_executable(movistartv_epg ${SRC_SOURCES})

target_link_libraries(movistartv_epg generic movistartv)
