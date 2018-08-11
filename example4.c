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

int64_t m_iSeekOffset = -1;
int64_t m_iLastFirstFfmpegByteOffset = -1;
int64_t m_iCurrentTs = -1;
int64_t m_iOffset = -1;
int m_bIsSeeked = 0;

long fe_read_seek(long filepos) {
    int ret = 0;
    int64_t fspos = 0;
    int64_t l_lSeekPos = 0;
    int64_t minus = filepos;
    AVRational time_base = m_pFormatCtx->streams[m_iAudioStream]->time_base;

    minus = (int64_t)((double)filepos / 2304);
    minus *= 2304;

    if( minus >= (2304 * 4) ) {
        minus -=  (2304 * 4);

    } else {
        minus = 0;
    }

    fspos = (int64_t) round(fe_convert_fromByteOffset((double)minus, 44100, &time_base));
    m_iCurrentTs = filepos;

    m_iOffset = 0;

    avcodec_flush_buffers(m_pCodecCtx);

    ret = avformat_seek_file(m_pFormatCtx,
                             m_iAudioStream,
                             0,
                             fspos,
                             fspos,
                             AVSEEK_FLAG_BACKWARD);


    avcodec_flush_buffers(m_pCodecCtx);

    if (ret < 0) {
        printf("fe_read_seek: Seek ERROR ret(%d) filepos(%ld)\n",
               ret,
               filepos);
        return 0;
    }

    l_lSeekPos = (int64_t) round(fe_convert_toByteOffset(
                                     m_pFormatCtx->streams[m_iAudioStream]->cur_dts, 44100, &time_base));

    m_iLastFirstFfmpegByteOffset = l_lSeekPos;


    m_iSeekOffset = m_iOffset = (int64_t) round(fe_convert_toByteOffset((double)(fspos - m_pFormatCtx->streams[m_iAudioStream]->cur_dts),
                                44100, &time_base));

    m_iSeekOffset = (int64_t) round((double)l_lSeekPos / 2304);
    m_iSeekOffset *= 2304;

    printf("fe_read_seek: (RBP: %ld/%.4f) Req Byte: %ld (Sec: %.4f PTS %ld) Real PTS %ld (Sec: %.4f Byte: %ld)\n",
           filepos,
           (double)((double)filepos / (double)88200),
           minus,
           (double)((double)minus / (double)88200),
           fspos,
           m_pFormatCtx->streams[m_iAudioStream]->cur_dts, m_pFormatCtx->streams[m_iAudioStream]->cur_dts * av_q2d(m_pFormatCtx->streams[m_iAudioStream]->time_base),
           (int64_t) round(fe_convert_toByteOffset((double)m_pFormatCtx->streams[m_iAudioStream]->cur_dts, 44100, &time_base)));

    m_bIsSeeked = 1;

    return filepos;
}


unsigned int fe_read_frame(char *buffer, int size) {
    AVPacket l_SPacket;
#if LIBAVCODEC_VERSION_INT >= 3616100
    AVFrame *l_pFrame = av_frame_alloc();
#else
    AVFrame *l_pFrame = avcodec_alloc_frame();
#endif
    int l_iCopySize = size * 2;

    int m_bReadLoop = 0;
    int l_iReadBytes = 0;

    double l_fCurrentFFMPEGPosSec = 0;
    double l_fCurrentFFMPEGPosByte = 0;

    int ret = 0;
    int64_t l_iOffset = 0;
    int64_t l_iReadedBytes = 0;
    int64_t l_iCopiedBytes = 0;

    // Just make sure everything is zeroed before use
    // Needless but..
    memset(buffer, 0x00, size);

    l_SPacket.data = NULL;
    l_SPacket.size = 0;
    av_init_packet(&l_SPacket);

    //while (readByteArray.size() < needed)
    while (!m_bReadLoop) {
        if (av_read_frame(m_pFormatCtx, &l_SPacket) >= 0) {
            if (l_SPacket.stream_index == m_iAudioStream) {

// If we have FFMpeg version which is less than 3.2 then we use older implementation
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 48, 0)
                ret = avcodec_decode_audio4(m_pCodecCtx, l_pFrame, &l_iFrameFinished, &l_SPacket);

                if (ret <= 0) {
#else

                // AVERROR(EAGAIN) means that we need to feed more
                // That we can decode Frame or Packet
                do {
                    do {
                        ret = avcodec_send_packet(m_pCodecCtx, &l_SPacket);
                    } while(ret == AVERROR(EAGAIN));

                    if(ret == AVERROR_EOF || ret == AVERROR(EINVAL)) {
                        printf("AVERROR(EAGAIN): %d, AVERROR_EOF: %d, AVERROR(EINVAL): %d\n", AVERROR(EAGAIN), AVERROR_EOF, AVERROR(EINVAL));
                        printf("fe_read_frame: Frame getting error (%d)!\n", ret);
                        return 0;
                    }

                    ret = avcodec_receive_frame(m_pCodecCtx, l_pFrame);
                } while(ret == AVERROR(EAGAIN));

                if(ret == AVERROR_EOF || ret == AVERROR(EINVAL)) {
#endif

                    // An error or EOF occured,index break out and return what
                    // we have so far.
                    printf("AVERROR(EAGAIN): %d, AVERROR_EOF: %d, AVERROR(EINVAL): %d\n", AVERROR(EAGAIN), AVERROR_EOF, AVERROR(EINVAL));
                    printf("fe_read_frame: EOF or some othere decoding error (%d)!\n", ret);
                    return 0;
                }

// If we have FFMpeg version which is less than 3.2 then we use older implementation
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 48, 0)

                //frame->
                if (l_iFrameFinished) {
#endif
                    fe_resample_do(l_pFrame);
                    l_iReadBytes = av_samples_get_buffer_size(NULL, m_pCodecCtx->channels,
                                   l_pFrame->nb_samples,
                                   m_pCodecCtx->sample_fmt, 1);

                    l_fCurrentFFMPEGPosSec = l_SPacket.pts * av_q2d(m_pFormatCtx->streams[m_iAudioStream]->time_base);

                    l_fCurrentFFMPEGPosByte = fe_convert_toByteOffset((double)l_SPacket.pts, 44100,
                                              &m_pFormatCtx->streams[m_iAudioStream]->time_base);


                    if( m_iCurrentTs > l_fCurrentFFMPEGPosByte && l_iOffset == 0) {
                        l_iOffset = m_iCurrentTs - (int64_t) round(l_fCurrentFFMPEGPosByte);
                        printf("fe_read_frame: Offset %ld = %ld - %ld\n",
                               l_iOffset,
                               m_iCurrentTs,
                               (int64_t) round(l_fCurrentFFMPEGPosByte));
                        l_iOffset *= 2;
                    }

                    if( l_iOffset >= l_iReadBytes && m_pOut == NULL) {
                        l_iOffset -= l_iReadBytes;
                        printf("fe_read_frame: Offset left/readed: %ld/%ld (was: %ld) (PTS: %ld/%ld)\n",
                               l_iOffset,
                               (long int)l_iReadBytes,
                               (l_iOffset + l_iReadBytes),
                               (long int)l_SPacket.pts,
                               (int64_t) round(l_fCurrentFFMPEGPosByte));
                        continue;

                    }  else if( l_iOffset >= m_pOutSize) {
                        l_iOffset -= m_pOutSize;
                        printf("fe_read_frame: Resample Offset left/readed: %ld/%ld (was: %ld) (PTS: %ld/%ld)\n",
                               l_iOffset,
                               (long int) m_pOutSize,
                               (long int) (l_iOffset + m_pOutSize),
                               (long int) l_SPacket.pts,
                               (long int) round(l_fCurrentFFMPEGPosByte));
                        continue;
                    }

                    printf("fe_read_frame: Cur Sec %.4f (Byte: %ld) Got bytes: (%d @ PTS %ld) (buffered: %ld/%ld)\n",
                           l_fCurrentFFMPEGPosSec,
                           (long int) round(l_fCurrentFFMPEGPosByte),
                           l_iReadBytes,
                           (long int) l_SPacket.pts,
                           (long int) l_iReadedBytes,
                           (long int) l_iCopySize);

                    l_iReadedBytes += (l_iReadBytes / 2);

                    if( m_pOut != NULL ) {

                        if( (m_pOutSize - l_iOffset) < l_iCopySize ) {
                            printf("fe_read_frame: Resampled Copy Size: %ld Readed: %ld Offset: %ld Copied: %ld\n",
                                   (long int)l_iCopySize,
                                   (long int)m_pOutSize,
                                   (long int)l_iOffset,
                                   (long int)(m_pOutSize - l_iOffset));
                            memcpy(buffer, m_pOut + l_iOffset, (m_pOutSize - l_iOffset));
                            buffer += (m_pOutSize - l_iOffset);
                            l_iCopiedBytes += (m_pOutSize - l_iOffset);

                        } else {
                            printf("fe_read_frame: Resampled Last Copy Size: %ld Readed: %ld Offset: %ld\n",
                                   (long int)l_iCopySize,
                                   (long int)m_pOutSize,
                                   (long int)l_iOffset);
                            memcpy(buffer, m_pOut, l_iCopySize);
                            l_iCopiedBytes += l_iCopySize;
                        }

                        l_iCopySize -= m_pOutSize - l_iOffset;
                        m_pOutSize = 0;
                        free(m_pOut);
                        m_pOut = NULL;

                    } else {
                        if( (l_iReadBytes - l_iOffset) < l_iCopySize ) {
                            printf("fe_read_frame: Copy Size: %ld Readed: %ld Offset: %ld Copied: %ld\n",
                                   (long int)l_iCopySize,
                                   (long int)l_iReadBytes,
                                   (long int)l_iOffset,
                                   (long int)(l_iReadBytes - l_iOffset));
                            memcpy(buffer, l_pFrame->data[0] + l_iOffset, (l_iReadBytes - l_iOffset));
                            buffer += (l_iReadBytes - l_iOffset);
                            l_iCopiedBytes += (l_iReadBytes - l_iOffset);

                        } else {
                            printf("fe_read_frame: Last Copy Size: %ld Readed: %ld Offset: %ld\n",
                                   (long int)l_iCopySize,
                                   (long int)l_iReadBytes,
                                   (long int)l_iOffset);
                            memcpy(buffer, l_pFrame->data[0], l_iCopySize);
                            l_iCopiedBytes += l_iCopySize;
                        }

                        l_iCopySize -= l_iReadBytes - l_iOffset;
                    }

                    l_iOffset = 0;

                    if( l_iCopySize <= 0) {
                        m_bReadLoop = 1;
                    }

// If we have FFMpeg version which is less than 3.2 then we use older implementation
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 48, 0)

                } else {
                    printf("fe_read_frame: libavcodec 'avcodec_decode_audio4' didn't succeed or frame not finished (File could also just end!)\n");
                }

#endif
            }

        } else {
            printf("fe_read_frame: libavcodec 'av_read_frame' didn't succeed!\n");
            //break;
        }

// If we have FFMpeg version which is less than 3.2 then we use older implementation
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(57, 48, 0)
        av_free_packet(&l_SPacket);
#else
        av_packet_unref(&l_SPacket);
#endif
        l_SPacket.data = NULL;
        l_SPacket.size = 0;
        av_init_packet(&l_SPacket);

    }

    av_free( l_SPacket.data );
    l_SPacket.data = NULL;
    l_SPacket.size = 0;

#if LIBAVFORMAT_VERSION_INT < 3613028
    avcodec_get_frame_defaults(l_pFrame);
#else
    av_frame_unref(l_pFrame);
#endif

    printf("fe_read_frame: Readed bytes %ld Copied bytes: %ld\n", l_iReadedBytes, l_iCopiedBytes);
    return l_iCopiedBytes;
}

