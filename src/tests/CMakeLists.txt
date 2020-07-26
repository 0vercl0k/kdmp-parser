# Axel '0vercl0k' Souchet - July 25 2020
find_package(Python3 COMPONENTS Interpreter)
add_custom_target(
    NativeTests ALL
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests.py --bindir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)