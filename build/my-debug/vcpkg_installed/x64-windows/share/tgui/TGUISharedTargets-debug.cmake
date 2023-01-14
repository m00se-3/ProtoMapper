#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "tgui" for configuration "Debug"
set_property(TARGET tgui APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(tgui PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/tgui-d.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "sfml-graphics"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/tgui-d.dll"
  )

list(APPEND _cmake_import_check_targets tgui )
list(APPEND _cmake_import_check_files_for_tgui "${_IMPORT_PREFIX}/debug/lib/tgui-d.lib" "${_IMPORT_PREFIX}/debug/bin/tgui-d.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
