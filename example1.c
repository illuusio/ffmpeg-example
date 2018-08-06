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

int fe_decode_open(char *filename) {
    int i = -1;
    AVDictionary *l_iFormatOpts = NULL;

    printf("fe_decode_open: Decode audio file %s\n",
           filename);

    m_pFormatCtx = avformat_alloc_context();

// Enable this to use old slow MP3 Xing TOC
#ifndef CODEC_ID_MP3

    if ( LIBAVFORMAT_VERSION_INT > 3540580 ) {
        printf("fe_decode_open: Set usetoc to have old way of XING TOC reading (libavformat version: '%d')\n", LIBAVFORMAT_VERSION_INT);
        av_dict_set(&l_iFormatOpts, "usetoc", "0", 0);
    }

#endif

    // Open file and make m_pFormatCtx
    if (avformat_open_input(&m_pFormatCtx, filename, NULL, &l_iFormatOpts) != 0) {
        printf("fe_decode_open: cannot open with 'avformat_open_input': %s\n",
               filename);
        return -1;
    }

#ifndef CODEC_ID_MP3

    if ( LIBAVFORMAT_VERSION_INT > 3540580 && l_iFormatOpts != NULL ) {
        av_dict_free(&l_iFormatOpts);
    }

#endif

#if LIBAVCODEC_VERSION_INT < 3622144
    m_pFormatCtx->max_analyze_duration = 999999999;
#endif

    // Retrieve stream information
    if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0) {
        printf("fe_decode_open: cannot open find info '%s'\n",
               filename);
        printf("As documentation says this is pretty normal. So this not show stopper!\n");
    }

    av_dump_format(m_pFormatCtx, 0, filename, 0);

    // Find the first video stream
    m_iAudioStream = -1;

    printf("fe_decode_open: File got streams: %d\n", m_pFormatCtx->nb_streams);

    for (i = 0; i < m_pFormatCtx->nb_streams; i++) {
// If we have FFMpeg version which is less than 3.2 then we use older implementation
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 48, 0)
        if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
#else

        if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
#endif
            m_iAudioStream = i;
            break;
        }
    }

    if (m_iAudioStream == -1) {
        printf("fe_decode_open: cannot find an audio stream: cannot open %s",
               filename);
        return -1;
    }

    // Find the decoder for the audio stream
    // If we have FFMpeg version which is less than 3.2 then we use older implementation
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 48, 0)

    if (!(m_pCodec = avcodec_find_decoder(m_pFormatCtx->streams[m_iAudioStream]->codec_id))) {
#else

    if (!(m_pCodec = avcodec_find_decoder(m_pFormatCtx->streams[m_iAudioStream]->codecpar->codec_id))) {
#endif
        printf("fe_decode_open: cannot find a decoder for %s\n",
               filename);
        return -1;
    }

// If we have FFMpeg version which is less than 3.2 then we use older implementation
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 48, 0)
    // Get a pointer to the codec context for the video stream
    m_pCodecCtx = m_pFormatCtx->streams[m_iAudioStream]->codec;

#else
    // Get a pointer to the codec context for the video stream
    //m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
    m_pCodecCtx = avcodec_alloc_context3(NULL);

    // Add stream parameters to context
    if(avcodec_parameters_to_context(m_pCodecCtx, m_pFormatCtx->streams[m_iAudioStream]->codecpar)) {
        printf("fe_decode_open: cannot add Codec parameters: %s\n",
               filename);
        return -1;
    }

// This is not needed anymore above FFMpeg version 4.0
#if LIBAVCODEC_VERSION_INT < 3805796
    // Se timebase correct
    av_codec_set_pkt_timebase(m_pCodecCtx, m_pFormatCtx->streams[m_iAudioStream]->time_base);
#endif

    // Make sure that Codecs are identical or  avcodec_open2 fails.
    m_pCodecCtx->codec_id = m_pCodec->id;
#endif

    if(!m_pCodecCtx) {
        printf("fe_decode_open: cannot get 'AVCodecContext'\n");
        return -1;
    }

    if (avcodec_open2(m_pCodecCtx, m_pCodec, NULL) < 0) {
        printf("fe_decode_open: cannot open with 'avcodec_open2' codec_id: %d Audio stream id: %d: %s\n",
               m_pFormatCtx->streams[m_iAudioStream]->codecpar->codec_id,
               m_iAudioStream, filename
              );
        return -1;
    }

    printf("fe_decode_open: PCM Length is: %f (Bytes: %ld)\n",
           (double)(m_pFormatCtx->duration / AV_TIME_BASE),
           (int64_t)round((double)(m_pFormatCtx->duration / AV_TIME_BASE) * (44100 * 4)));
    m_lPcmLength = (int64_t)round((double)(m_pFormatCtx->duration / AV_TIME_BASE) * (44100 * 4));

    return 0;
}
