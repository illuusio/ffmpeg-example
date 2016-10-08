FFmpeg-example
==============

[FFmpeg](http://ffmpeg.org/) 0.11-3.2 or [Libav](http://libav.org/) 0.8-11 example application with byte exact reading 

FFmpeg/Libav example program
============================

This program is example how to use FFmpeg or Libav libarary with Byte exact reading (Audio only!). Tested with 'gcc'.
Currently it only outputs 'out-for.pcm' and 'out-rev.pcm' files that are in Stereo 16-bit raw audio files (s16) that
can be opened with Audacity audio editor or similar that imports raw audio.

Compling with CMake
-------------------
You need to have FFmpeg or Libav installed. You need [CMake](http://cmake.org/) and [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/).
<pre>
cmake .
</pre>
If everything goes fine
<pre>
make
</pre>

Compiling with autotools
------------------------
You need to have FFmpeg or Libav installed. You need thave [GNU autotools](http://www.gnu.org/savannah-checkouts/gnu/automake/manual/html_node/Autotools-Introduction.html), [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) and [libtools](http://www.gnu.org/software/libtool/) 
installed.
<pre>
./autogen.sh
./configure
</pre>
If everything goes fine
<pre>
make
</pre>

Compiling with scons
--------------------
You need to have FFmpeg or Libav installed. You need have [SCons](http://www.scons.org/) and [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/).
<pre>
scons
</pre>

Compiling with script
---------------------
You need to have FFmpeg or Libav installed. You need have [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/).
<pre>
./compile-pkgconfig.sh
</pre>

Running application
===================
Currently tested files are OGG/Vorbis,MP3 and MP4 (also acc).

<pre>
./FFmpeg-example-bin audio-file
</pre>

You get as output 'out-for.pcm' and 'out-rev.pcm' files that contains played forward byte exact and
reverse played file (as audio is normal!)
