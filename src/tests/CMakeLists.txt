# Axel '0vercl0k' Souchet - April 18 2020
enable_testing()

#
# Download & extract the test data
#
set(TEST_DATA_HASH )
set(TEST_DATA_FILENAME "kdmp-parser-testdatas.zip")
set(TEST_DATA_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../..")

include(ExternalProject)
ExternalProject_Add(
    TestData
    URL https://github.com/0vercl0k/kdmp-parser/releases/download/v0.1/testdatas.zip
    URL_HASH MD5=bc7ac069cad00bce24bcadefb7df150d
    PREFIX ${TEST_DATA_PREFIX}
    DOWNLOAD_NAME ${TEST_DATA_FILENAME}
    DOWNLOAD_NO_PROGRESS 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

add_custom_target(DownloadAndExtract DEPENDS TestData)

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

cmake_path(GET TEST_DATA_FILENAME STEM TEST_DATA_NAME)
foreach(SOURCE_FILE ${TEST_SOURCE_FILES})
    cmake_path(GET SOURCE_FILE STEM TEST_EXECUTABLE_NAME)
    add_executable(${TEST_EXECUTABLE_NAME} ${SOURCE_FILE})
    add_dependencies(${TEST_EXECUTABLE_NAME} kdmp-parser)
    target_compile_definitions(${TEST_EXECUTABLE_NAME} PUBLIC TEST_DATA_DIR="${TEST_DATA_PREFIX}/${TEST_DATA_NAME}")
    target_link_libraries(${TEST_EXECUTABLE_NAME} PUBLIC kdmp-parser Catch2::Catch2WithMain)
    add_test(NAME ${TEST_EXECUTABLE_NAME} COMMAND $<TARGET_FILE:${TEST_EXECUTABLE_NAME}>)
    add_dependencies(${TEST_EXECUTABLE_NAME} DownloadAndExtract)
    install(FILES $<TARGET_FILE:${TEST_EXECUTABLE_NAME}> DESTINATION tests)
endforeach()
