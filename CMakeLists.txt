cmake_minimum_required(VERSION 3.0)
project(Polynomials C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra")

# Wskazujemy pliki źródłowe.
set(CALC_SOURCE_FILES
        src/main.c
        src/poly/poly.c src/poly/poly.h
        src/poly/io/poly_io.c src/poly/io/poly_io.h
        src/poly/poly_stack.c src/poly/poly_stack.h
        src/calc/calc_error.c src/calc/calc_error.h
        src/calc/calc_poly.c src/calc/calc_poly.h
        src/calc/calc_command.c src/calc/calc_command.h
        src/poly/io/numeric_parser.c src/poly/io/numeric_parser.h
        src/poly/io/poly_parser.c src/poly/io/poly_parser.h
        )

set(TEST_SOURCE_FILES
        test/poly_test.c
        src/poly/poly.c
        src/poly/poly.h
        test/poly_data.h)

add_executable(poly ${CALC_SOURCE_FILES})

add_executable(test ${TEST_SOURCE_FILES})
set_target_properties(test PROPERTIES OUTPUT_NAME poly_test)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif (DOXYGEN_FOUND)



