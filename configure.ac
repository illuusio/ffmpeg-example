AC_PREREQ([2.57])
AC_INIT([ffmpeg-example], [131027], [https://github.com/illuusio], [ffmpeg-example], [https://github.com/illuusio])
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG_C
AC_PROG_CC
AM_PROG_LIBTOOL

C_OPTIONS="-Wall -Werror -std=c99 -fno-strict-aliasing -g -D__LIBAVRESAMPLE__ -std=c99 -I."

AC_SUBST(C_OPTIONS)

PKG_CHECK_MODULES(FFMPEG, libavresample libavcodec libavformat libavutil)

AC_OUTPUT([Makefile])
