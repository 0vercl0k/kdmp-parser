# Axel '0vercl0k' Souchet - July 25 2020
include(FindPython3)
add_custom_target(
    NativeTests ALL
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests.py --bindir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)