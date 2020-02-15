CMAKE_MINIMUM_REQUIRED(VERSION 3.10 )
PROJECT(poly_decomp)

set(CMAKE_CXX_STANDARD 11)
set(TARGET_NAME ${PROJECT_NAME})

include_directories(
        include
)

set(HEADERS
        include/Point.h
        include/common.h
        include/polyDecomp.h
)

set( SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Point.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/polyDecomp.cpp
)

# expose the source list to any calling parent (hacky)
set( poly_decomp_SOURCES ${SOURCES} PARENT_SCOPE )

# Build the main library
add_library(${TARGET_NAME}
        ${HEADERS}
        ${SOURCES}
        )

target_include_directories(${TARGET_NAME}
        INTERFACE src)

