# Axel '0vercl0k' Souchet - April 18 2020
enable_testing()

#
# Download & extract the test data
#
set(TEST_DATA_PATH ${CMAKE_CURRENT_LIST_DIR}/testdatas)
include(ExternalProject)
ExternalProject_Add(
    TestData
    URL https://github.com/0vercl0k/kdmp-parser/releases/download/v0.1/testdatas.zip
    URL_HASH MD5=bc7ac069cad00bce24bcadefb7df150d
    SOURCE_DIR ${TEST_DATA_PATH}
    DOWNLOAD_NO_PROGRESS 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

#
# Build and link Catch2 library dependency automatically if not already found
#
include(FetchContent)
FetchContent_Declare(
    Catch2
    URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.4.0.zip
    URL_HASH MD5=c426e77d4ee0055410bc930182959ae5
)
FetchContent_MakeAvailable(Catch2)

#
# Build all test files
#
list(APPEND TEST_SOURCE_FILES
    tests_version.cc
    tests_parser.cc
)

foreach(SOURCE_FILE ${TEST_SOURCE_FILES})
    cmake_path(GET SOURCE_FILE STEM TEST_EXECUTABLE_NAME)
    add_executable(${TEST_EXECUTABLE_NAME} ${SOURCE_FILE})
    add_dependencies(${TEST_EXECUTABLE_NAME} kdmp-parser)
    target_compile_definitions(${TEST_EXECUTABLE_NAME} PUBLIC TEST_DATA_DIR="${TEST_DATA_PATH}")
    target_link_libraries(${TEST_EXECUTABLE_NAME} PUBLIC kdmp-parser Catch2::Catch2WithMain)
    add_test(NAME ${TEST_EXECUTABLE_NAME} COMMAND $<TARGET_FILE:${TEST_EXECUTABLE_NAME}>)
    add_dependencies(${TEST_EXECUTABLE_NAME} TestData)
    install(FILES $<TARGET_FILE:${TEST_EXECUTABLE_NAME}> DESTINATION tests)
endforeach()
