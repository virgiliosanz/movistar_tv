add_subdirectory(movistartv)
add_subdirectory(core)
add_subdirectory(bstrlib)

#--- executable
file(GLOB SRC_SOURCES ./ *.c *.h)
add_executable(movistartv_epg ${SRC_SOURCES})
target_link_libraries(movistartv_epg movistartv core bstrlib ${THIRDPARTY_LIBRARIES})
