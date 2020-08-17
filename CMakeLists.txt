cmake_minimum_required(VERSION 3.16)
project(helloflight)
set(CMAKE_CXX_STANDARD 14)

message("Build type " ${CMAKE_BUILD_TYPE})

get_cmake_property(vars CACHE_VARIABLES)
foreach (var ${vars})
    get_property(currentHelpString CACHE "${var}" PROPERTY HELPSTRING)
    if ("${currentHelpString}" MATCHES "No help, variable specified on the command line." OR "${currentHelpString}" STREQUAL "")
        # message("${var} = [${${var}}]  --  ${currentHelpString}") # uncomment to see the variables being processed
        list(APPEND CL_ARGS "-D${var}=${${var}}")
    endif ()
endforeach ()

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(arrow
        PREFIX external/arrow
        GIT_REPOSITORY "https://github.com/apache/arrow.git"
        GIT_TAG master
        INSTALL_COMMAND ""
        CMAKE_ARGS ${CL_ARGS} -DARROW_COMPUTE=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DARROW_PARQUET=ON
            -DPARQUET_BUILD_EXAMPLES=ON
            -DARROW_COMPUTE=ON
            -DARROW_FLIGHT=ON
            -DARROW_BUILD_TESTS=ON
            -DARROW_BUILD_INTEGRATION=ON
        SOURCE_SUBDIR cpp)
message("CL_ARGS" ${CL_ARGS})

ExternalProject_Get_Property(arrow SOURCE_DIR)
ExternalProject_Get_Property(arrow BINARY_DIR)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOW)

set(ARROW_INCLUDE_DIR "${SOURCE_DIR}/cpp/src")
set(ARROW_INCLUDE_GEN_DIR "${BINARY_DIR}/src")
include_directories(${ARROW_INCLUDE_DIR} ${ARROW_INCLUDE_GEN_DIR})
SET(ARROW_LIB_DIR ${BINARY_DIR}/${BUILD_TYPE_LOW})

add_library(arrow_shared SHARED IMPORTED)
set_target_properties(arrow_shared PROPERTIES IMPORTED_LOCATION ${ARROW_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}arrow${CMAKE_SHARED_LIBRARY_SUFFIX})
add_library(arrow_flight_shared SHARED IMPORTED)
set_target_properties(arrow_flight_shared PROPERTIES IMPORTED_LOCATION ${ARROW_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}arrow_flight${CMAKE_SHARED_LIBRARY_SUFFIX})
add_library(arrow_bundled_dependencies STATIC IMPORTED)
set_target_properties(arrow_bundled_dependencies PROPERTIES IMPORTED_LOCATION ${ARROW_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}arrow_bundled_dependencies${CMAKE_STATIC_LIBRARY_SUFFIX})

add_executable(helloflight main.cpp)
add_dependencies(helloflight arrow)
target_link_libraries(helloflight arrow_shared arrow_flight_shared arrow_bundled_dependencies)