#link_directories(${CMAKE_CURRENT_BINARY_DIR}/../src/lib)

file(GLOB TESTS "test_*.c")
foreach(T_FILE ${TESTS})
  get_filename_component(T_NAME ${T_FILE} NAME_WE)
  add_executable(${T_NAME} ${T_FILE})
  target_link_libraries(${T_NAME} mtv core ${THIRDPARTY_LIBRARIES} -lpthread)
  add_test(NAME ${T_NAME} COMMAND ${T_NAME})
endforeach(T_FILE ${TESTS})

configure_file(xmltv.dtd ${CMAKE_CURRENT_BINARY_DIR}/xmltv.dtd COPYONLY)

