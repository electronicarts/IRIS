function(AddCoverage target)
  find_program(LCOV_PATH lcov REQUIRED)
  find_program(GENHTML_PATH genhtml REQUIRED)
  message("Add coverage to: " ${target})
  message(${LCOV_PATH})
  message("BIN DIR: " ${CMAKE_BINARY_DIR})
  message("SRC DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(coverage
    COMMENT "Running coverage for ${target}..."
    COMMAND ${LCOV_PATH} -d . --zerocounters
    COMMAND $<TARGET_FILE:${target}>
    COMMAND ${LCOV_PATH} -d . --capture -o coverage.info
    COMMAND ${LCOV_PATH} -r coverage.info '/usr/include/*' '${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/*' '${CMAKE_CURRENT_SOURCE_DIR}/*' '/usr/local/include/*'
                         -o filtered.info
    COMMAND ${GENHTML_PATH} -o coverage filtered.info --legend
    COMMAND rm -rf coverage.info filtered.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endfunction()