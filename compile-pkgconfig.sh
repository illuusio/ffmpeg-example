#!/bin/sh
#RESAMPLER=__LIBAVRESAMPLE__
#RESAMPLER=__LIBSWRESAMPLE__
RESAMPLER=__FFMPEGOLDAPI__
# RESAMPLER_LIB=avresample
# RESAMPLER_LIB=swresample
RESAMPLER_LIB=

echo "$(pkg-config --libs --cflags ${RESAMPLER_LIB} libavcodec libavformat libavutil) -lm -g -D${RESAMPLER} -std=c99 -I."


gcc -o ffmpeg-example-bin example1.c example2.c example3.c example4.c main.c \
$(pkg-config --libs --cflags ${RESAMPLER_LIB} libavcodec libavformat libavutil) -lm -g -D${RESAMPLER} -std=c99 -I.

