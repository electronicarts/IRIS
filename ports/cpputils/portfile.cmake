vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL C:\\dev\\cpputils #https://gitlab.ea.com/eacc-tech/cpputils
    REF c3c2276e60ea77c6c2abcfb2c73572ed3849f0c2
    HEAD_REF dev-blanca/cmake-refactor
)

vcpkg_cmake_configure(
  SOURCE_PATH "${SOURCE_PATH}"
  OPTIONS
    -DEXPORT_CPPUTILS=1            
)
  
vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
    CONFIG_PATH cmake/
)

vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/Copyright.txt" COMMENT "Install copyright")
