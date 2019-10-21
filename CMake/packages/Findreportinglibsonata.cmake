# =============================================================================
# Copyright (C) 2016-2019 Blue Brain Project
#
# See top-level LICENSE file for details.
# =============================================================================

# Findreportinglibsonata
# -------------
#
# Find reportinglibsonata
#
# Find the SONATA implementation of the reportinglib Blue Brain HPC utils library
#
# Using reportinglib:
#
# ::
#
#   find_package(reportinglibsonata REQUIRED)
#   include_directories(${reportinglib_INCLUDE_DIRS})
#   target_link_libraries(foo ${reportinglib_LIBRARIES})
#
# This module sets the following variables:
#
# ::
#
#   reportinglibsonata_FOUND - set to true if the library is found
#   reportinglib_INCLUDE_DIRS - list of required include directories
#   reportinglib_LIBRARIES - list of libraries to be linked

# UNIX paths are standard, no need to write.
find_path(reportinglib_INCLUDE_DIR reportinglib/records.h)
find_library(reportinglib_LIBRARY reportinglibsonata)
get_filename_component(reportinglib_LIB_DIR ${reportinglib_LIBRARY} DIRECTORY)

# Checks 'REQUIRED', 'QUIET' and versions.
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(reportinglibsonata
  FOUND_VAR reportinglibsonata_FOUND
  REQUIRED_VARS reportinglib_INCLUDE_DIR reportinglib_LIBRARY reportinglib_LIB_DIR)

