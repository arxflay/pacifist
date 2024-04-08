include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchCPM.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM/CPM.cmake")
set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_LIST_DIR}/third_party/CPM")
CPMFindPackage(NAME arxengine
    GITHUB_REPOSITORY arxflay/arxengine
    VERSION 0.1
    GIT_TAG v0.1
    OPTIONS 
        "BUILD_SHARED_LIBS OFF"
        "ARX_DISABLE_INSTALL ON"
)

