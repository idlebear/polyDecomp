CMAKE_MINIMUM_REQUIRED(VERSION 3.10 )
PROJECT(polyDecomp)

set(CMAKE_CXX_STANDARD 11)
set(TARGET_NAME ${PROJECT_NAME})

# option(${PROJECT_NAME}_xxxx "Use conan" OFF)

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
set( polyDecomp_SOURCES ${SOURCES} PARENT_SCOPE )

# Build the main library
add_library(${TARGET_NAME}
        ${HEADERS}
        ${SOURCES}
        )

target_include_directories(${TARGET_NAME}
        INTERFACE src)

install(TARGETS ${TARGET_NAME}
        ARCHIVE DESTINATION lib)