#!/bin/sh
RESAMPLER=__LIBAVRESAMPLE__ 
#RESAMPLER=__LIBSWRESAMPLE__ 
#RESAMPLER=__FFMPEGOLDAPI__ 
RESAMPLER_LIB=avresample
# RESAMPLER_LIB=swresample
# RESAMPLER_LIB=  

gcc -o ffmpeg-example-bin example1.c example2.c example3.c example4.c main.c -l${RESAMPLER_LIB} -lavcodec -lavformat -lavutil -lm \
-g -D${RESAMPLER} -std=c99 -I.
