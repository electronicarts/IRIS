# Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

cmake_minimum_required (VERSION 3.21.0)
project(utils VERSION 1.0.0 LANGUAGES CXX)

message("BUILD UTILS LIBRARY")

# ---------------------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------------------
option(BUILD_SHARED_LIBS "Build as a shared library" OFF)
option(EXPORT_UTILS "Export and install library" ON)

# ---------------------------------------------------------------------------------------
# Compiler config
# ---------------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(BUILD_SHARED_LIBS)
    message("Build as shared library")
    if(WIN32)
        message("Export DLL symbols on Windows")
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
endif()

# ---------------------------------------------------------------------------------------
# Utils library
# ---------------------------------------------------------------------------------------

set(PUBLIC_HEADERS
     "include/utils/BaseLog.h"
     "include/utils/FrameConverter.h"
     "include/utils/JsonWrapper.h"
)
source_group("Header Files" FILES ${PUBLIC_HEADERS})

set(SOURCE_FILES      
    "src/BaseLog.cpp"
    "src/FrameConverter.cpp"
)
source_group("Source Files" FILES ${SOURCE_FILES})


# ---------------------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------------------s

find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)


# ---------------------------------------------------------------------------------------
# Utils library
# ---------------------------------------------------------------------------------------

add_library(${PROJECT_NAME} ${SOURCE_FILES})

if(BUILD_SHARED_LIBS)
    if(WIN32)
        target_compile_definitions(${PROJECT_NAME} PUBLIC UTILS_SHARED PRIVATE UTILS_EXPORT)
    endif()
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

set_target_properties(${PROJECT_NAME} PROPERTIES 
			PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog 
                                      PRIVATE nlohmann_json::nlohmann_json opencv_core
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    # where the top-level project will look for the library's public headers
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	# where external projects will look for the library's public headers
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" 
)


# ---------------------------------------------------------------------------------------
# Export & Install
# ---------------------------------------------------------------------------------------

if(EXPORT_UTILS)
message("Export Utils")

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