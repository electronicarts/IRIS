# Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.


project(IrisApp)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "main.cpp")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/appsettings.json ${CMAKE_CURRENT_BINARY_DIR}/appsettings.json COPYONLY)

# Included libraries
target_link_libraries(${PROJECT_NAME} iris)

# add the binary tree to the search path for include files
# so that we will find library headers
target_include_directories(${PROJECT_NAME} PUBLIC
                          "${PROJECT_SOURCE_DIR}/include"
                           )

# Copy needed dll files into executable when building iris as dll
if(BUILD_SHARED_LIBS)    
    message("Copy dynamic libraries into IrisApp directory")
    file(GLOB_RECURSE DLL_FILES ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll)
    file(COPY ${DLL_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    set(IRIS_DLL iris.dll)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug") # set DEBUG_POSTFIX
        set(IRIS_DLL irisd.dll)
    endif()

    add_custom_command(TARGET ${PROJECT_NAME} 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/${IRIS_DLL}
        ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copying Iris DLL to build directory"                    
    )
endif()
