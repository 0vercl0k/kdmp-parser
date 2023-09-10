# Axel '0vercl0k' Souchet - April 18 2020
enable_testing()
cmake_path(SET TEST_DATA_DIR NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/../../kdmp-parser-testdatas")
message(STATUS ${TEST_DATA_DIR})
list(APPEND TEST_SOURCE_FILES

    # Test cases
    tests_parser.cc

    # Performance tests
    # TODO
)

foreach(SOURCE_FILE ${TEST_SOURCE_FILES})
    cmake_path(GET SOURCE_FILE STEM TEST_EXECUTABLE_NAME)
    add_executable(${TEST_EXECUTABLE_NAME} ${SOURCE_FILE})
    add_dependencies(${TEST_EXECUTABLE_NAME} kdmp-parser)
    target_compile_definitions(${TEST_EXECUTABLE_NAME} PUBLIC TEST_DATA_DIR="${TEST_DATA_DIR}")
    target_link_libraries(${TEST_EXECUTABLE_NAME} PUBLIC kdmp-parser)

    if(WIN32)
        target_link_options(${TEST_EXECUTABLE_NAME} PUBLIC /SUBSYSTEM:Console)
    endif(WIN32)

    add_test(NAME ${TEST_EXECUTABLE_NAME} COMMAND $<TARGET_FILE:${TEST_EXECUTABLE_NAME}>)
    install(FILES $<TARGET_FILE:${TEST_EXECUTABLE_NAME}> DESTINATION tests)
endforeach()
