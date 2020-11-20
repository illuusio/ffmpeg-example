project(ffmpeg-examples)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

#versioning
set(CPACK_PACKAGE_VERSION_MAJOR 20)
set(CPACK_PACKAGE_VERSION_MINOR 11)
set(CPACK_PACKAGE_VERSION_PATCH 19)
set(FFMPEGEXAMPLE_PROTO_VER 0)
set(FFMPEGEXAMPLE_VERSION
    "${CPACK_PACKAGE_VERSION_MAJOR}.
    ${CPACK_PACKAGE_VERSION_MINOR}.
    ${CPACK_PACKAGE_VERSION_PATCH}")

set(target_dir "bin")

# set default flags values for all projects
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
# set(CMAKE_C_FLAGS_RELEASE
#     "-w -Os -fomit-frame-pointer ${CMAKE_C_FLAGS_RELEASE}")
# set(CMAKE_C_FLAGS_DEBUG
#     "-Wall -O0 -g -DDEBUG ${CMAKE_C_FLAGS_DEBUG}")
# set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
# set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
# set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -D__LIBAVRESAMPLE__ -std=c99")

add_executable(ffmpeg-example-bin
  example1.c
  example2.c
  example3.c
  example4.c
  main.c)

include_directories(".")
# find_package(FFmpeg REQUIRED)
find_package(LIBAV REQUIRED)

# LIBAV_FOUND - system has ffmpeg or libav
# LIBAV_INCLUDE_DIR - the ffmpeg include directory
# LIBAV_LIBRARIES
# LIBAV_LIBAVCODEC
# LIBAV_LIBAVFORMAT
# LIBAV_LIBAVUTIL
if(LIBAV_FOUND)
  message("Found FFMPEG/LibAV libraries")
  include_directories(${LIBAV_INCLUDE_DIR})
  target_link_libraries(ffmpeg-example-bin ${LIBAV_LIBRARIES})
else()
  message("Can't find libavcodec, libavformat or libavutil. Add them!")
endif()

target_link_libraries(ffmpeg-example-bin "-lm")
target_link_libraries(ffmpeg-example-bin "-lavresample")


# CPack variables
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FFMPEG example CMake file")
set(CPACK_PACKAGE_VENDOR "Tuukka Pasanen <tuukka.pasanen@ilmi.fi>")
set(CPACK_PACKAGE_FILE_NAME "ffmpeg-example-${HEDGEWARS_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "ffmpeg-example-src-${HEDGEWARS_VERSION}")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_PACKAGE_EXECUTABLES "ffmpeg-example-bin")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_INSTALL_DIRECTORY
  "ffmpeg-example
  ${CPACK_PACKAGE_VERSION_MAJOR}.
  ${CPACK_PACKAGE_VERSION_MINOR}.
  ${CPACK_PACKAGE_VERSION_PATCH}")

if(WIN32 AND NOT UNIX)
set(CPACK_NSIS_DISPLAY_NAME "FFMPEG Example")
set(CPACK_NSIS_HELP_LINK "https://github.com/illuusio")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/illuusio")
set(CPACK_NSIS_CONTACT "tuukka.pasanen@ilmi.fi")
set(CPACK_NSIS_MODIFY_PATH OFF)
set(CPACK_GENERATOR "ZIP;NSIS")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "ffmpeg-example")
else()
set(CPACK_STRIP_FILES "bin/ffmpeg-example")
endif()

set(CPACK_SOURCE_IGNORE_FILES
"~"
"\\\\.hg"
"\\\\.svn"
"\\\\.exe$"
"\\\\.a$"
"\\\\.dll$"
"\\\\.xcf$"
"\\\\.cxx$"
"\\\\.db$"
"\\\\.dof$"
"\\\\.layout$"
"\\\\.zip$"
"\\\\.gz$"
"\\\\.bz2$"
"\\\\.tmp$"
"\\\\.core$"
"\\\\.sh$"
"\\\\.sifz$"
"\\\\.svg$"
"\\\\.svgz$"
"\\\\.ppu$"
"\\\\.psd$"
"\\\\.o$"
"Makefile"
"Doxyfile"
"CMakeFiles"
"debug"
"release$"
"Debug$"
"Release$"
"proto\\\\.inc$"
"hwconsts\\\\.cpp$"
"playlist\\\\.inc$"
"CPack"
"cmake_install\\\\.cmake$"
"config\\\\.inc$"
"CMakeCache\\\\.txt$"
"^${CMAKE_CURRENT_SOURCE_DIR}/misc/liblua"
"^${CMAKE_CURRENT_SOURCE_DIR}/install_manifest.txt"
"^${CMAKE_CURRENT_SOURCE_DIR}/CMakeCache.txt"
)

include(CPack)

