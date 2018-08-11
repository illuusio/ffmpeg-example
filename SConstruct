# Compile ffmpeg-example-bin
# Because FFmpeg 4.0 Depricates AVResample you have to change __LIBAVRESAMPLE__
# to __LIBSWRESAMPLE__ to make this compile
env = Environment(CPPPATH = ['.'], CCFLAGS='-Wall -Werror -std=c99 -fno-strict-aliasing -I/usr/include/ffmpeg -g -D__LIBSWRESAMPLE__ -std=c99')
# env = Environment(CPPPATH = ['.'], CCFLAGS='-Wall -Werror -std=c99 -fno-strict-aliasing -I/usr/include/ffmpeg -g -D__LIBAVRESAMPLE__ -std=c99')
#env.Program(target = "ffmpeg-example-bin", LIBS=["avresample" ,"avcodec", "avformat", "avutil", "m"],
# source = ["example1.c", "example2.c", "example3.c", "example4.c", "main.c"])
env.Program(target = "ffmpeg-example-bin", LIBS=["swresample" ,"avcodec", "avformat", "avutil", "m"],
 source = ["example1.c", "example2.c", "example3.c", "example4.c", "main.c"])

