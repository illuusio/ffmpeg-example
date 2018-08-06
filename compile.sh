#!/bin/bash

RESAMPLER=_"_LIBAVRESAMPLE__"
#RESAMPLER="__LIBSWRESAMPLE__"
#RESAMPLER="__FFMPEGOLDAPI__"
RESAMPLER_LIB="avresample"
# RESAMPLER_LIB="swresample"
# RESAMPLER_LIB=

FFMPEG_INCLUDE=/usr/include/ffmpeg
FFMPEG_VERSION=$(pkg-config --modversion libavcodec | tr -d '.')

if [ ${FFMPEG_VERSION} -ge 5818100 ]
then
    echo "FFMpeg Version is bigger than 4.0.1. Which to SWResample because AVResample is depricated."
    RESAMPLER_LIB="swresample"
    RESAMPLER="__LIBSWRESAMPLE__"
fi

gcc -o ffmpeg-example-bin example1.c example2.c example3.c example4.c main.c -l${RESAMPLER_LIB} \
    -lavcodec -lavformat -lavutil -lm -g -D${RESAMPLER} -std=c99 -I. -I${FFMPEG_INCLUDE}
