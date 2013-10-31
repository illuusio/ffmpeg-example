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

#include <example1.h>

AVFormatContext *m_pFormatCtx = NULL;
AVInputFormat *m_pIformat = NULL;
AVCodecContext *m_pCodecCtx = NULL;
int m_iAudioStream = -1;
AVCodec *m_pCodec = NULL;
int64_t m_lPcmLength = 0;

int fe_decode_open(char *filename)
{
    int i = -1;

    printf("fe_decode_open: Decode audio file %s\n",
           filename);

    m_pFormatCtx = avformat_alloc_context();

    // Open file and make m_pFormatCtx
    if (avformat_open_input(&m_pFormatCtx,filename, NULL, NULL)!=0) {
        printf("fe_decode_open: cannot open: %s\n",
               filename);
        return -1;
    }

    m_pFormatCtx->max_analyze_duration = 999999999;

    // Retrieve stream information
    if (avformat_find_stream_info(m_pFormatCtx, NULL)<0) {
        printf("fe_decode_open: cannot open %s\n",
               filename);
        return -1;
    }

    av_dump_format(m_pFormatCtx, 0, filename, 0);

    // Find the first video stream
    m_iAudioStream=-1;

    for (i=0; i<m_pFormatCtx->nb_streams; i++) {
        if (m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO) {
            m_iAudioStream=i;
            break;
        }
    }

    if (m_iAudioStream==-1) {
        printf("fe_decode_open: cannot find an audio stream: cannot open %s",
               filename);
        return -1;
    }

    // Get a pointer to the codec context for the video stream
    m_pCodecCtx=m_pFormatCtx->streams[m_iAudioStream]->codec;

    // Find the decoder for the audio stream
    if (!(m_pCodec=avcodec_find_decoder(m_pCodecCtx->codec_id))) {
        printf("fe_decode_open: cannot find a decoder for %s\n",
               filename);
        return -1;
    }


    if (avcodec_open2(m_pCodecCtx, m_pCodec, NULL)<0) {
        printf("fe_decode_open:  cannot open %s\n",
               filename);
        return -1;
    }

    printf("fe_decode_open: PCM Length is: %f (Bytes: %ld)\n",
           (double)(m_pFormatCtx->duration / AV_TIME_BASE),
           (int64_t)round((double)(m_pFormatCtx->duration / AV_TIME_BASE) * (44100 * 4)));
    m_lPcmLength = (int64_t)round((double)(m_pFormatCtx->duration / AV_TIME_BASE) * (44100 * 4));

    return 0;
}
