set(ARXENGINE_FOUND FALSE)

find_path(ARXENGINE_INCLUDE_DIR arxengine/GameApp.h)
mark_as_advanced(ARXENGINE_INCLUDE_DIR) #mark as non visible in gui
find_library(ARXENGINE_LIBRARY NAMES arxengine arxengined)
mark_as_advanced(ARXENGINE_LIBRARY)


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(arxengine
    FOUND_VAR ARXENGINE_FOUND
    REQUIRED_VARS ARXENGINE_LIBRARY ARXENGINE_INCLUDE_DIR)

if(${ARXENGINE_FOUND})
    add_library(arxengine UNKNOWN IMPORTED)
    #set headers and lib
    set_property(TARGET arxengine APPEND PROPERTY
        IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(
      arxengine PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${ARXENGINE_INCLUDE_DIR}"
    )
    set_target_properties(
      arxengine PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C++" # mark as C++ library
      IMPORTED_LOCATION "${ARXENGINE_LIBRARY}" # location
    )
endif()
