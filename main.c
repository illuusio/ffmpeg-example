// Copyright (c) 2013, Tuukka Pasanen <tuukka.pasanen@ilmi.fi>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define FFMPEG_EXAMPLE_STEP 32768

#include "example1.h"
#include "example2.h"
#include "example3.h"
#include "example4.h"

int main(int argc, char * argv[]) {
    //int64_t i = 0;
    int64_t j = 0;

//fe_read_seek(0);
    char l_strBuffer[(FFMPEG_EXAMPLE_STEP * 2)];
    char l_strBuffer2[(FFMPEG_EXAMPLE_STEP * 2)];
    FILE *l_pOutFile = NULL;
    int l_iReadSize = 0;
    //int64_t l_iRead = 200;
    //int64_t l_iStart = 1000;
    //int64_t l_iStartCur = 0;
    //int64_t l_iReadCur = 0;
    //int64_t l_iStartUp = 0;
    //int64_t l_iStartDown = 0;
    //int64_t l_iSeekDirection = 0;
    //int64_t l_iSeekStep = 50;

    int64_t l_lSeekStartPoint = 0;
    //int64_t l_lSeekMiddleLowPoint = 0;
    //int64_t l_lSeekMiddleHighPoint = 0;
    //int64_t l_lSeekEndPoint = 0;
    
// FFMPEG uses SOMETHING_MICRO > 100 and Avconv under
#if LIBAVCODEC_VERSION_MICRO >= 100
    printf("Using Library: FFmpeg\n");
#else
    printf("Using Library: Avconv\n");
#endif

    printf("libavcodec version %d.%d.%d (%d)\n", LIBAVCODEC_VERSION_MAJOR,
           LIBAVCODEC_VERSION_MINOR,
           LIBAVCODEC_VERSION_MICRO,
           LIBAVCODEC_VERSION_INT);
    printf("libavformat version %d.%d.%d (%d)\n", LIBAVFORMAT_VERSION_MAJOR,
           LIBAVFORMAT_VERSION_MINOR,
           LIBAVFORMAT_VERSION_MICRO,
           LIBAVFORMAT_VERSION_INT);


    if( argc < 2 ) {
        printf("Usage: ffmpeg-example-bin audio.mp3/wav/flac/ogg/opus/m4v/aac\n");
        printf("\nOpen with audacity file: 'out-f.pcm' (import raw data)\n");
        printf("Stereo 16 Bit / Little Endian / 2 channels\n");
        return 0;
    }

    av_register_all();
    avcodec_register_all();
    fe_decode_open(argv[1]);
    fe_resample_open(m_pCodecCtx->sample_fmt, AV_SAMPLE_FMT_S16);

    l_pOutFile = fopen("out-for.pcm", "w+");
    
    // l_lSeekMiddleLowPoint = l_lSeekMiddleHighPoint = (m_lPcmLength / 2);
   
    
    memset(l_strBuffer, 0x99, (FFMPEG_EXAMPLE_STEP * 2));
    memset(l_strBuffer2, 0x88, (FFMPEG_EXAMPLE_STEP * 2));

    for( j = 0; j < 2; j ++) {
            printf("main: Ask reading %ld (23040 bytes)\n",l_lSeekStartPoint);
            fe_read_seek(l_lSeekStartPoint);
            //l_iReadSize = fe_read_frame(l_strBuffer, FFMPEG_EXAMPLE_STEP);
            l_iReadSize = fe_read_frame(l_strBuffer, 23040);
            // fseek( l_pOutFile, l_lSeekStartPoint, SEEK_SET);
            fwrite(l_strBuffer, l_iReadSize, 1, l_pOutFile);
            fwrite(l_strBuffer2, 1024, 1, l_pOutFile);
	    //l_lSeekStartPoint += FFMPEG_EXAMPLE_STEP;
	    l_lSeekStartPoint += 23040;

      
    }

    
    
    /*for( i = l_iStart; i < l_iRead; i ++) {
        fwrite(l_strBuffer, 4608, 1, l_pOutFile);
    }

    // We start reading at the half of file our wanted area..
    l_iStartCur = l_iStart + (l_iRead / 2);
    l_iReadCur = l_iRead / l_iSeekStep;

    //av_opt_show2(&m_pFormatCtx->iformat->priv_class, NULL, AV_OPT_FLAG_DECODING_PARAM, 0);

    // This example stars from middle and then seeks forward and
    // backward
    for( j = 0; j < 50; j++ ) {
        printf("Start: %ld Read: %ld\n", l_iStartCur, l_iReadCur);

        for( i = 0; i <= l_iReadCur; i ++) {
            memset(l_strBuffer, 0x00, 65355);
            printf("Loop: %ld/%ld (@%ld of %ld)\n", j, i, (2304 * (i + l_iStartCur)), m_lPcmLength);
            fe_read_seek(2304 * (i + l_iStartCur));
            l_iReadSize = fe_read_frame(l_strBuffer, 2304);
            fseek( l_pOutFile, (2304 * 2) * (i + l_iStartCur), SEEK_SET);
            fwrite(l_strBuffer, l_iReadSize, 1, l_pOutFile);
            // If you like to see what we just read eanble this..
            //memset(l_strBuffer, 0x77, 65355);
            // fwrite(l_strBuffer, 100, 1, l_pOutFile);
        }

        if( l_iStartDown == 0 || l_iStartDown > l_iStartCur ) {
            l_iStartDown = l_iStartCur;
        }

        if( l_iStartUp == 0 || l_iStartUp < l_iStartCur ) {
            l_iStartUp = l_iStartCur;
        }

        if( l_iSeekDirection == 0 ) {
            l_iSeekDirection = 1;
            l_iStartCur = l_iStartDown - (l_iStartDown / 2);
            l_iReadCur = l_iStartDown - l_iStartCur;

        } else {
            l_iSeekDirection = 0;
            l_iReadCur = l_iRead / l_iSeekStep;
            l_iStartCur = l_iStartUp + l_iReadCur;
        }

        if( l_iStartCur < l_iStart) {
            l_iStartCur = l_iStart;
            l_iReadCur = l_iStartDown - l_iStartCur;
        }
    }
    */
 
    fclose(l_pOutFile);

    if (m_pCodecCtx != NULL) {
        avcodec_close(m_pCodecCtx);
        avformat_close_input(&m_pFormatCtx);
        m_pCodecCtx = NULL;
        m_pFormatCtx = NULL;
    }

    // This is example that reads from end to start..
    //
    //fe_decode_open(argv[1]);
    //fe_resample_open(m_pCodecCtx->sample_fmt, AV_SAMPLE_FMT_S16);
    //
    //l_pOutFile = fopen("out-rev.pcm", "w+");
    //
    //for( i = l_iStart; i < l_iRead; i ++) {
    //    memset(l_strBuffer, 0x33, 65355);
    //    fwrite(l_strBuffer, 4608, 1, l_pOutFile);
    //}
    //
    //
    //for( i = l_iRead; i >= l_iStart; i --) {
    //    memset(l_strBuffer, 0x00, 65355);
    //    printf("Loop: %ld\n", i);
    //    fe_read_seek(2304 * i);
    //    l_iReadSize = fe_read_frame(l_strBuffer, 2304);
    //    fseek( l_pOutFile, (2304 * 2) * (i - l_iStart), SEEK_SET);
    //    fwrite(l_strBuffer, l_iReadSize, 1, l_pOutFile);
    //    memset(l_strBuffer, 0x77, 65355);
    //    // fwrite(l_strBuffer, 100, 1, l_pOutFile);
    //}
    //
    //fclose(l_pOutFile);

    if (m_pSwrCtx != NULL) {
#ifndef __FFMPEGOLDAPI__

#ifdef __LIBAVRESAMPLE__
        avresample_close(m_pSwrCtx);
#else
        swr_free(&m_pSwrCtx);
#endif

#else
        audio_resample_close(m_pSwrCtx);
#endif
    }

    return 0;
}
