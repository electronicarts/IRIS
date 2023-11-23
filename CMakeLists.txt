# Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

cmake_minimum_required (VERSION 3.21.0)
project(iris VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/)

# ---------------------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------------------
option(BUILD_TESTS "Build library tests" OFF)
option(BUILD_EXAMPLE_APP "Build Iris example console application" OFF)
option(EXPORT_IRIS "Export and install library" ON)
option(BUILD_SHARED_LIBS "Build iris as a shared library" OFF)
option(BUILD_COVERAGE "Builds code coverage target" OFF)

if (NOT UNIX)
    set(BUILD_COVERAGE OFF)
    message("Code coverage can only be built in Linux systems")
endif()

# ---------------------------------------------------------------------------------------
# Iris Library
# ---------------------------------------------------------------------------------------

set(PUBLIC_HEADERS
    "include/iris/VideoAnalyser.h"
    "include/iris/Configuration.h"
    "include/iris/Log.h"
    "include/iris/Result.h"
    "include/iris/TotalFlashIncidents.h"
)

source_group("Public header files" FILES ${PUBLIC_HEADERS})

set(SOURCE_FILES
    "src/VideoAnalyser.cpp"
    "src/FrameRgbConverter.cpp"
    "src/Flash.cpp"
    "src/RelativeLuminance.cpp"
    "src/RedSaturation.h"
    "src/RedSaturation.cpp"
    "src/CDLuminance.cpp"
    "src/TransitionEvaluator.cpp"
    "src/Configuration.cpp"
    "src/ConfigurationParams.h"
    "src/Flash.h" 
    "src/RelativeLuminance.h"
    "src/CDLuminance.h"
    "src/FrameRgbConverter.h" 
    "src/TransitionEvaluator.h" 
    "src/FrameData.h"
    "src/FlashDetection.h" 
    "src/FlashDetection.cpp" 
    "src/IrisFrame.h"
    "src/Log.cpp"
    "src/PatternDetection.h"
    "src/PatternDetection.cpp"
    "src/PhotosensitivityDetector.h"
)

source_group("Source files" FILES ${SOURCE_FILES}) 

# Dependencies
find_package(OpenCV CONFIG REQUIRED)
add_subdirectory ("utils")

if(BUILD_SHARED_LIBS)
    message("BUILD SHARED LIBRARIES")
    if(WIN32)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
        message("Export symbols")
    endif()
endif()

if(BUILD_COVERAGE)
    message("BUILD CODE COVERAGE")
    SET(GCC_COVERAGE_COMPILE_FLAGS "-g -O0 -coverage -fprofile-arcs -ftest-coverage")
    SET(GCC_COVERAGE_LINK_FLAGS    "-coverage -lgcov")
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
endif()

# Library definition
add_library(${PROJECT_NAME} ${PUBLIC_HEADERS} ${SOURCE_FILES})

if(BUILD_SHARED_LIBS)
    if(WIN32)
        target_compile_definitions(${PROJECT_NAME} PUBLIC IRIS_SHARED PRIVATE IRIS_EXPORT)
    endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_include_directories(${PROJECT_NAME}
	PUBLIC
    # where the top-level project will look for the library's public headers
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	# where external projects will look for the library's public headers
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_compile_options(${PROJECT_NAME}
  PRIVATE $<$<CONFIG:Release>:-Wall>
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
			PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

target_link_libraries(${PROJECT_NAME} 
                PUBLIC
                    utils
                    ${OpenCV_LIBS}
)

# ---------------------------------------------------------------------------------------
# Build Example App
# ---------------------------------------------------------------------------------------

if(BUILD_EXAMPLE_APP)
	message("Build Iris example app")
	add_subdirectory ("example")
endif()

# ---------------------------------------------------------------------------------------
# Build Tests
# ---------------------------------------------------------------------------------------
if(BUILD_TESTS)
	message("Build tests")
	enable_testing()
	add_subdirectory("test/Iris.Tests")
	add_subdirectory("test/AddressSanitizer.Tests")
endif()

# ---------------------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------------------

if(EXPORT_IRIS)
message("Export Iris")

include(GNUInstallDirs)
set(namespace ${PROJECT_NAME})

# install the target and create export-set
install(TARGETS ${PROJECT_NAME}
    EXPORT "${PROJECT_NAME}Targets"
    # these variable get default values from GNUInstallDirs
    #RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
    #LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    #ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
)

# generate and install export file
install(EXPORT "${PROJECT_NAME}Targets"
    FILE "${PROJECT_NAME}Targets.cmake"
    NAMESPACE ${namespace}::
    DESTINATION cmake
)

include(CMakePackageConfigHelpers)

# generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION "${version}"
    COMPATIBILITY AnyNewerVersion
)

# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION cmake
)

# install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION cmake
)

# generate the export targets for the build tree
export(EXPORT "${PROJECT_NAME}Targets"
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Targets.cmake"
    NAMESPACE ${namespace}::
)

endif()