# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "AttendanceServer_autogen"
  "CMakeFiles\\AttendanceServer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AttendanceServer_autogen.dir\\ParseCache.txt"
  )
endif()
