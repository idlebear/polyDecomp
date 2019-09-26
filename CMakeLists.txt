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


###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
  LIBRARIES ${TARGET_NAME}
  CATKIN_DEPENDS 
  DEPENDS 
)

## Mark executables and/or libraries for installation
install(TARGETS ${PROJECT_NAME}
   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

## Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME}/
   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
   FILES_MATCHING PATTERN "*.h"
   PATTERN ".svn" EXCLUDE
 )