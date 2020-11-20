AC_PREREQ([2.57])
AC_INIT([ffmpeg-example], [201119], [https://github.com/illuusio/ffmpeg-example], [ffmpeg-example], [https://github.com/illuusio/ffmpeg-example])
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG_C
AC_PROG_CC
AM_PROG_LIBTOOL

RESAMPLE=__LIBAVRESAMPLE__
RESAMPLELIB=libavresample

AC_ARG_ENABLE([avresample], [  --disable-avresample    Disable use of avresample])
AC_ARG_ENABLE([swresample], [  --enable-swresample     Disable use of avresample and use libswresample instead])

PKG_CHECK_MODULES([FFMEG_LIBAVCODEC],[libavcodec],
  [ac_libavcodec_version=$($PKG_CONFIG --modversion libavcodec | tr -d '.')],
  [ac_libavcodec_version=;])

if test "${ac_libavcodec_version}" -ge "5818100"; then
    echo "FFMpeg Version is bigger than 4.0.1. Which to SWResample because AVResample is depricated."
    RESAMPLELIB="libswresample"
    RESAMPLE="__LIBSWRESAMPLE__"
fi


AS_IF([test "x$enable_avresample" == "xno"], [
  dnl As we don't need education we disable this feature
  echo "** We are using OLD FFMPEG resampling. Not working with FFMPEG 1.x and above (Libav 9) **"
  RESAMPLE=__FFMPEGOLDAPI__
  RESAMPLELIB=
])

AS_IF([test "x$enable_swresample" == "xyes"], [
  dnl As we don't need education we disable this feature
  echo "** We are using SWResample good luck **"
  RESAMPLE=__LIBSWRESAMPLE__
  RESAMPLELIB=libswresample
])


C_OPTIONS="-Wall -Werror -std=c99 -fno-strict-aliasing -g -D${RESAMPLE} -std=c99 -I."

AC_SUBST(C_OPTIONS)


PKG_CHECK_MODULES(FFMPEG, ${RESAMPLELIB} libavcodec libavformat libavutil)

AC_OUTPUT([Makefile])
