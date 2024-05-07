find_package(Gkdi)

set(TEST_NAME
    gkdi_test
)

add_executable(${TEST_NAME} gkdi_test.c)

add_test(NAME gkdi COMMAND ${TEST_NAME})
target_include_directories(${TEST_NAME} PRIVATE ${GKDI_INCLUDE_DIR})
target_link_directories(${TEST_NAME} PRIVATE ${GKDI_LIBRARY_DIR})
target_link_libraries(${TEST_NAME} PRIVATE ${GKDI_LIBRARY})
