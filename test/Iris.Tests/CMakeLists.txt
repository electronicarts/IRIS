# Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

project(iris_tests)

set(HEADER_FILES
    "include/IrisLibTest.h"
)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
   "src/FrameRgbConverterTest.cpp" 
   "src/FlashTest.cpp"
   "src/RedSaturationTests.cpp"
   "src/RelativeLuminanceTest.cpp"
   "src/FlashDetectionTests.cpp"
   "src/PatternDetectionTests.cpp" 
   "src/VideoAnalysisTests.cpp"
   "src/TransitionTrackerTests.cpp"
   "src/FrameManagerTests.cpp"
)

source_group("Source Files" FILES ${SOURCE_FILES})

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC iris GTest::gtest_main)

option(ASAN_ENABLED "Build this target with AddressSanitizer" ON)



if(ASAN_ENABLED)
  if(NOT MSVC AND NOT APPLE)
    target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
    endif()
endif()


# add the binary tree to the search path for include files
# so that we will find library headers
target_include_directories(${PROJECT_NAME} PRIVATE
                          "${CMAKE_SOURCE_DIR}/include"
                          "${CMAKE_SOURCE_DIR}/src"
                          "${PROJECT_SOURCE_DIR}/include"
                           )


# Copies all appsettings files into the executable directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/appsettings.json ${CMAKE_CURRENT_BINARY_DIR}/appsettings.json COPYONLY)


# Copies all frame files into the executable directory
add_custom_target(copy_data ALL 
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        ${CMAKE_CURRENT_BINARY_DIR}/data
        COMMENT "Copying test data into current binary directory")

add_dependencies(${PROJECT_NAME} copy_data)

if(BUILD_SHARED_LIBS)    
    message("Copy dynamic libraries into iris tests directory")
    file(GLOB_RECURSE DLL_FILES ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll)
    file(COPY ${DLL_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

if(BUILD_COVERAGE)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/appsettings.json ${CMAKE_BINARY_DIR}/appsettings.json COPYONLY)
    add_custom_target(copy_data_bin_dir ALL 
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        ${CMAKE_BINARY_DIR}/data
        COMMENT "Copying test data into binary directory")
    add_dependencies(${PROJECT_NAME} copy_data_bin_dir)

    include(Coverage)
    AddCoverage(iris_tests)
endif()

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
#gtest_discover_tests(${PROJECT_NAME} PROPERTIES DISCOVERY_TIMEOUT 5000)
