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

#include "example1.h"
#include "example2.h"
#include "example3.h"
#include "example4.h"

int main(int argc, char * argv[])
{
    int i = 0;
//fe_read_seek(0);
    char l_strBuffer[65355];
    FILE *l_pOutFile = NULL;
    int l_iReadSize = 0;
    int l_iRead = 2000;
    int l_iStart = 500;

    av_register_all();
    avcodec_register_all();
    fe_decode_open(argv[1]);
    fe_resample_open(m_pCodecCtx->sample_fmt, AV_SAMPLE_FMT_S16);


    l_pOutFile = fopen("out-for.pcm", "w+");

    for( i = l_iStart; i <= l_iRead; i ++) {
        memset(l_strBuffer, 0x00, 65355);
        printf("Loop: %d\n", i);
        fe_read_seek(2304 * i);
        l_iReadSize = fe_read_frame(l_strBuffer, 2304);
        fwrite(l_strBuffer, l_iReadSize, 1, l_pOutFile);
        memset(l_strBuffer, 0x77, 65355);
        // fwrite(l_strBuffer, 100, 1, l_pOutFile);
    }

    fclose(l_pOutFile);

    if (m_pCodecCtx != NULL) {
        avcodec_close(m_pCodecCtx);
        avformat_close_input(&m_pFormatCtx);
        m_pCodecCtx = NULL;
        m_pFormatCtx = NULL;
    }
    fe_decode_open(argv[1]);
    fe_resample_open(m_pCodecCtx->sample_fmt, AV_SAMPLE_FMT_S16);

    l_pOutFile = fopen("out-rev.pcm", "w+");

    for( i = l_iStart; i < l_iRead; i ++) {
        memset(l_strBuffer, 0x33, 65355);
        fwrite(l_strBuffer, 4608, 1, l_pOutFile);
    }


    for( i = l_iRead; i >= l_iStart; i --) {
        memset(l_strBuffer, 0x00, 65355);
        printf("Loop: %d\n", i);
        fe_read_seek(2304 * i);
        // fe_read_seek((rand() % 2304) * i * 10);
        // fe_read_seek(2304 * i * 10);
        l_iReadSize = fe_read_frame(l_strBuffer, 2304);
        fseek( l_pOutFile, (2304 * 2) * (i - l_iStart), SEEK_SET);
        fwrite(l_strBuffer, l_iReadSize, 1, l_pOutFile);
        memset(l_strBuffer, 0x77, 65355);
        // fwrite(l_strBuffer, 100, 1, l_pOutFile);
    }

    fclose(l_pOutFile);

//for( i = 0; i < 2; i ++) {
//    fe_read_seek(2304 * (i * 10));
//}

    if (m_pCodecCtx != NULL) {
        avcodec_close(m_pCodecCtx);
        avformat_close_input(&m_pFormatCtx);
    }

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
