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

// AVCodecContext *m_pCodecCtx = NULL;
enum AVSampleFormat m_pOutSampleFmt;
enum AVSampleFormat m_pInSampleFmt;


#ifndef __FFMPEGOLDAPI__

// Please choose to use libavresample.. people
// Compile it now.. but because macports doesn't
// Support both.. damn!
#ifdef __LIBAVRESAMPLE__
AVAudioResampleContext *m_pSwrCtx = NULL;
#else
SwrContext *m_pSwrCtx = NULL;
#endif
uint8_t *m_pOut = NULL;
#else
ReSampleContext *m_pSwrCtx = NULL;
short *m_pOut = NULL;
#endif

unsigned int m_pOutSize = -1;


int fe_resample_open(enum AVSampleFormat inSampleFmt,
                     enum AVSampleFormat outSampleFmt) {
    m_pOutSampleFmt = outSampleFmt;
    m_pInSampleFmt = inSampleFmt;


    // Some MP3/WAV don't tell this so make assumtion that
    // They are stereo not 5.1
    if (m_pCodecCtx->channel_layout == 0 && m_pCodecCtx->channels == 2) {
        m_pCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;

    } else if (m_pCodecCtx->channel_layout == 0 && m_pCodecCtx->channels == 1) {
        m_pCodecCtx->channel_layout = AV_CH_LAYOUT_MONO;

    } else if (m_pCodecCtx->channel_layout == 0 && m_pCodecCtx->channels == 0) {
        m_pCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
        m_pCodecCtx->channels = 2;
    }

    printf("fe_resample_open: Rate: %d Channels %d Channels: %d\n",
           (int) m_pCodecCtx->sample_rate,
           (int) m_pCodecCtx->channel_layout,
           (int) m_pCodecCtx->channels);

    // They make big change in FFPEG 1.1 before that every format just passed
    // s16 back to application from FFMPEG 1.1 up MP3 pass s16p (Planar stereo
    // 16 bit) MP4/AAC FLTP (Planar 32 bit float) and OGG also FLTP
    // (WMA same thing) If sample type aint' S16 (Stero 16) example FFMPEG 1.1
    // mp3 is s16p that ain't and mp4 FLTP (32 bit float)
    // NOT Going to work because MIXXX works with pure s16 that is not planar
    // GOOD thing is now this can handle allmost everything..
    // What should be tested is 44800 Hz downsample and 22100 Hz up sample.
    if ((inSampleFmt != outSampleFmt || m_pCodecCtx->sample_rate != 44100 ||
            m_pCodecCtx->channel_layout != AV_CH_LAYOUT_STEREO) && m_pSwrCtx == NULL) {
        if (m_pSwrCtx != NULL) {
            printf("fe_resample_open: Freeing Resample context\n");

// __FFMPEGOLDAPI__ Is what is used in FFMPEG < 0.10 and libav < 0.8.3. NO
// libresample available..
#ifndef __FFMPEGOLDAPI__

#ifdef __LIBAVRESAMPLE__
            avresample_close(m_pSwrCtx);
#else
            swr_free(&m_pSwrCtx);
#endif

#else
            audio_resample_close(m_pSwrCtx);
#endif
            m_pSwrCtx = NULL;
        }

        // Create converter from in type to s16 sample rate
#ifndef __FFMPEGOLDAPI__

#ifdef __LIBAVRESAMPLE__
        printf("fe_resample_open: ffmpeg: NEW FFMPEG API using libavresample\n");
        m_pSwrCtx = avresample_alloc_context();
#else
        printf("fe_resample_open: ffmpeg: NEW FFMPEG API using libswresample\n");
        m_pSwrCtx = swr_alloc();
#endif

        av_opt_set_int(m_pSwrCtx, "in_channel_layout", m_pCodecCtx->channel_layout, 0);
        av_opt_set_int(m_pSwrCtx, "in_sample_fmt", inSampleFmt, 0);
        av_opt_set_int(m_pSwrCtx, "in_sample_rate", m_pCodecCtx->sample_rate, 0);
        av_opt_set_int(m_pSwrCtx, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
        av_opt_set_int(m_pSwrCtx, "out_sample_fmt", outSampleFmt, 0);
        av_opt_set_int(m_pSwrCtx, "out_sample_rate", m_pCodecCtx->sample_rate, 0);

#else
        printf("fe_resample_open: ffmpeg: OLD FFMPEG API in use!\n");
        m_pSwrCtx = av_audio_resample_init(2,
                                           m_pCodecCtx->channels,
                                           m_pCodecCtx->sample_rate,
                                           m_pCodecCtx->sample_rate,
                                           outSampleFmt,
                                           inSampleFmt,
                                           16,
                                           10,
                                           0,
                                           0.8);

#endif

        if (!m_pSwrCtx) {
            printf("fe_resample_open: Can't init convertor!\n");
            return -1;
        }

#ifndef __FFMPEGOLDAPI__
        // If it not working let user know about it!
        // If we don't do this we'll gonna crash
#ifdef __LIBAVRESAMPLE__

        if (avresample_open(m_pSwrCtx) < 0) {
#else

        if (swr_init(m_pSwrCtx) < 0) {
#endif
            printf("fe_resample_open: Can't init convertor\n");
            m_pSwrCtx = NULL;
            return -1;
        }

#endif

    }

// If we have FFMpeg version which is less than 3.2 then we use older implementation
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 48, 0)
    printf("fe_resample_open: From Sample: %d Hz Sample format: %s",
           m_pFormatCtx->streams[m_iAudioStream]->codec->sample_rate,
           av_get_sample_fmt_name(inSampleFmt));
#else
    printf("fe_resample_open: From Sample: %d Hz Sample format: %s",
           m_pFormatCtx->streams[m_iAudioStream]->codecpar->sample_rate,
           av_get_sample_fmt_name(inSampleFmt));
#endif

    printf(" to 44100 Sample format: %s\n",
           av_get_sample_fmt_name(outSampleFmt));

    return 0;
}

unsigned int fe_resample_do(AVFrame *inframe) {

    if (m_pSwrCtx) {

#ifndef __FFMPEGOLDAPI__

#ifdef __LIBAVRESAMPLE__
#if LIBAVRESAMPLE_VERSION_MAJOR == 0
        void **l_pIn = (void **)inframe->extended_data;
#else
        uint8_t **l_pIn = (uint8_t **)inframe->extended_data;
#endif
#else
        uint8_t **l_pIn = (uint8_t **)inframe->extended_data;
#endif

// Left here for reason!
// Sometime in time we will need this!
//#else
//        int64_t l_lInReadBytes = av_samples_get_buffer_size(NULL, m_pCodecCtx->channels,
//                                 inframe->nb_samples,
//                                 m_pCodecCtx->sample_fmt, 1);
#endif

#ifndef __FFMPEGOLDAPI__
        int l_iOutBytes = 0;

#if __LIBAVRESAMPLE__
        int l_iOutSamples = av_rescale_rnd(avresample_get_delay(m_pSwrCtx) +
                                           inframe->nb_samples,
                                           m_pCodecCtx->sample_rate,
                                           m_pCodecCtx->sample_rate,
                                           AV_ROUND_UP);
#else
        int l_iOutSamples = av_rescale_rnd(swr_get_delay(m_pSwrCtx,
                                           m_pCodecCtx->sample_rate) +
                                           inframe->nb_samples,
                                           m_pCodecCtx->sample_rate,
                                           m_pCodecCtx->sample_rate,
                                           AV_ROUND_UP);
#endif
        int l_iOutSamplesLines = 0;

        // Alloc too much.. if not enough we are in trouble!
        if( av_samples_alloc(&m_pOut, &l_iOutSamplesLines, 2, l_iOutSamples,
                             m_pOutSampleFmt, 1) < 0 ) {
            printf("fe_resample_do: Alloc not succeedeed\n");
            return -1;
        }

#else
        int l_iOutSamples = av_rescale_rnd(inframe->nb_samples,
                                           m_pCodecCtx->sample_rate,
                                           m_pCodecCtx->sample_rate,
                                           AV_ROUND_UP);

        int l_iOutBytes =  av_samples_get_buffer_size(NULL, 2,
                           l_iOutSamples,
                           m_pOutSampleFmt, 1);


        m_pOut = (short *)malloc(l_iOutBytes * 2);
#endif

        int l_iLen = 0;
#ifndef __FFMPEGOLDAPI__

#ifdef __LIBAVRESAMPLE__

// OLD API (0.0.3) ... still NEW API (1.0.0 and above).. very frustrating..
// USED IN FFMPEG 1.0 (LibAV SOMETHING!). New in FFMPEG 1.1 and libav 9
#if LIBAVRESAMPLE_VERSION_INT <= 3
        // AVResample OLD
        l_iLen = avresample_convert(m_pSwrCtx, (void **)&m_pOut, 0, l_iOutSamples,
                                    (void **)l_pIn, 0, inframe->nb_samples);
#else
        //AVResample NEW
        l_iLen = avresample_convert(m_pSwrCtx, (uint8_t **)&m_pOut, 0, l_iOutSamples,
                                    (uint8_t **)l_pIn, 0, inframe->nb_samples);

#endif

#else

        // SWResample
        l_iLen = swr_convert(m_pSwrCtx, (uint8_t **)&m_pOut, l_iOutSamples,
                             (const uint8_t **)l_pIn, inframe->nb_samples);


#endif

        l_iOutBytes = av_samples_get_buffer_size(NULL, 2, l_iLen, m_pOutSampleFmt, 1);

#else
        l_iLen = audio_resample(m_pSwrCtx,
                                (short *)m_pOut, (short *)inframe->data[0],
                                inframe->nb_samples);

#endif

        if (l_iLen < 0) {
            printf("Sample format conversion failed!\n");
            return -1;
        }

        m_pOutSize = l_iOutBytes;
        return l_iOutBytes;

    } else {
        return 0;
    }

    return 0;
}
