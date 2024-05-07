find_package(Gkdi)

set(EXAMPLE_NAME
    gkdi_example
)

set(SOURCES
    main.cpp
)

add_executable(${EXAMPLE_NAME} ${SOURCES})
target_include_directories(${EXAMPLE_NAME} PRIVATE ${GKDI_INCLUDE_DIR})
target_link_directories(${EXAMPLE_NAME} PRIVATE ${GKDI_LIBRARY_DIR})
target_link_libraries(${EXAMPLE_NAME} PRIVATE ${GKDI_LIBRARY})
