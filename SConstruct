# Compile ffmpeg-example-bin
env = Environment(CPPPATH = ['.'], CCFLAGS='-Wall -Werror -std=c99 -fno-strict-aliasing -g -D__LIBAVRESAMPLE__ -std=c99')
env.Program(target = "ffmpeg-example-bin", LIBS=["avresample" ,"avcodec", "avformat", "avutil", "m"],
 source = ["example1.c", "example2.c", "example3.c", "example4.c", "main.c"])

