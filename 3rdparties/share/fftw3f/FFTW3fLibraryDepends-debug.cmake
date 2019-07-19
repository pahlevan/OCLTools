#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "FFTW3::fftw3f" for configuration "Debug"
set_property(TARGET FFTW3::fftw3f APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(FFTW3::fftw3f PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/fftw3f.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS FFTW3::fftw3f )
list(APPEND _IMPORT_CHECK_FILES_FOR_FFTW3::fftw3f "${_IMPORT_PREFIX}/debug/lib/fftw3f.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
