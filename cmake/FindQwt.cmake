#.rst:
# FindQwt
# -------
#
# Find the Qwt (Qt Widgets for Technical Applications) library.
#
# This module defines:
#
#   QWT_FOUND        - True if Qwt was found
#   QWT_INCLUDE_DIRS - Include directory for Qwt headers
#   QWT_LIBRARIES    - The Qwt libraries to link

set(QWT_SEARCH_HINTS
    "${QWT_DIR}"
    "${QWT_ROOT}"
    "C:/Qwt-6.1.5"
    "C:/qwt-6.1.5"
    "$ENV{QWT_ROOT}"
    "$ENV{QWT_DIR}"
)

find_path(QWT_INCLUDE_DIR
    NAMES qwt_plot.h
    HINTS ${QWT_SEARCH_HINTS}
    PATH_SUFFIXES include
    DOC "Qwt include directory"
)

find_library(QWT_LIBRARY_RELEASE
    NAMES qwt
    HINTS ${QWT_SEARCH_HINTS}
    PATH_SUFFIXES lib
    DOC "Qwt release library"
)

find_library(QWT_LIBRARY_DEBUG
    NAMES qwtd
    HINTS ${QWT_SEARCH_HINTS}
    PATH_SUFFIXES lib
    DOC "Qwt debug library"
)

include(FindPackageHandleStandardArgs)

set(QWT_TEST_LIBRARIES ${QWT_LIBRARY_RELEASE})
if(NOT QWT_TEST_LIBRARIES)
    set(QWT_TEST_LIBRARIES ${QWT_LIBRARY_DEBUG})
endif()

find_package_handle_standard_args(Qwt
    REQUIRED_VARS QWT_INCLUDE_DIR QWT_TEST_LIBRARIES
)

if(QWT_FOUND)
    set(QWT_INCLUDE_DIRS ${QWT_INCLUDE_DIR})
    if(QWT_LIBRARY_RELEASE AND QWT_LIBRARY_DEBUG)
        set(QWT_LIBRARIES debug ${QWT_LIBRARY_DEBUG} optimized ${QWT_LIBRARY_RELEASE})
    elseif(QWT_LIBRARY_RELEASE)
        set(QWT_LIBRARIES ${QWT_LIBRARY_RELEASE})
    else()
        set(QWT_LIBRARIES ${QWT_LIBRARY_DEBUG})
    endif()
endif()

mark_as_advanced(QWT_INCLUDE_DIR QWT_LIBRARY_RELEASE QWT_LIBRARY_DEBUG)