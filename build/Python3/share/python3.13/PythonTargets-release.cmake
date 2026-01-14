#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libpython-shared" for configuration "Release"
set_property(TARGET libpython-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libpython-shared PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libpython3.13.dylib"
  IMPORTED_SONAME_RELEASE "/Users/runner/work/portable-python/portable-python/build/python-install-3.13.9/lib/libpython3.13.dylib"
  )

list(APPEND _cmake_import_check_targets libpython-shared )
list(APPEND _cmake_import_check_files_for_libpython-shared "${_IMPORT_PREFIX}/lib/libpython3.13.dylib" )

# Import target "python" for configuration "Release"
set_property(TARGET python APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(python PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/python"
  )

list(APPEND _cmake_import_check_targets python )
list(APPEND _cmake_import_check_files_for_python "${_IMPORT_PREFIX}/bin/python" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
