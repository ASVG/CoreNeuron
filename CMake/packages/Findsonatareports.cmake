# =============================================================================
# Copyright (C) 2016-2019 Blue Brain Project
#
# See top-level LICENSE file for details.
# =============================================================================

# Findsonatareports
# -------------
#
# Find sonatareports
#
# Find the SONATA implementation of the reportinglib Blue Brain HPC utils library
#
# Using sonatareports:
#
# ::
#
#   find_package(sonatareports REQUIRED)
#   include_directories(${sonatareports_INCLUDE_DIR})
#   target_link_libraries(foo ${sonatareports_LIBRARY})
#
# This module sets the following variables:
#
# ::
#
#   sonatareports_FOUND - set to true if the library is found
#   sonatareports_INCLUDE_DIR - list of required include directories
#   sonatareports_LIBRARY - list of libraries to be linked

# UNIX paths are standard, no need to write.
find_path(sonatareports_INCLUDE_DIR reportinglib/records.h)
find_library(sonatareports_LIBRARY sonatareports)
get_filename_component(sonatareports_LIB_DIR ${sonatareports_LIBRARY} DIRECTORY)

# Checks 'REQUIRED', 'QUIET' and versions.
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(sonatareports
  FOUND_VAR sonatareports_FOUND
  REQUIRED_VARS sonatareports_INCLUDE_DIR sonatareports_LIBRARY sonatareports_LIB_DIR)

