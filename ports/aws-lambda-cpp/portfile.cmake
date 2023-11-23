vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL https://gitlab.ea.com/eacc-tech/aws-lambda-cpp
    REF e4f4c092dc7cd57c5d22a95959987a3770ef57ff # v0.2.7
    HEAD_REF master
)

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
)

vcpkg_cmake_install()

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)

vcpkg_copy_pdbs()

vcpkg_cmake_config_fixup(PACKAGE_NAME aws-lambda-runtime CONFIG_PATH lib/aws-lambda-runtime/cmake)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/lib/aws-lambda-runtime")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/lib/aws-lambda-runtime")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
