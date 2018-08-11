AM_CPPFLAGS = $(FFMPEG_CFLAGS) $(C_OPTIONS) -D_POSIX_SOURCE -D_USE_XOPEN -D_USE_MISC
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = COPYING compile-pkgconfig.sh compile.sh CMakeLists.txt CMakeModules/FindFFmpeg.cmake SConstruct autogen.sh m4/*


bin_PROGRAMS = ffmpeg-example-bin
# sbin_PROGRAMS = ffmpeg-example-bin
# noinst_PROGRAMS = ffmpeg-example-bin

ffmpeg_example_bin_SOURCES = example1.c example2.c example3.c example4.c main.c example1.h example2.h example3.h example4.h
ffmpeg_example_bin_LDFLAGS = $(FFMPEG_LIBS) -lm

