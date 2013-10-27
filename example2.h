// Copyright (c) 2013, Tuukka Pasanen <tuukka.pasanen@ilmi.fi>
// All rights reserved.
//
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

#ifndef FFMPEGEXAMPLE2_H
#define FFMPEGEXAMPLE2_H

#include <libavutil/mathematics.h>
#include <libavutil/opt.h>

// Thank you macports not providing libavresample for FFMPEG!
#ifdef __LIBAVRESAMPLE__
#include <libavresample/avresample.h>
#else
#include <libswresample/swresample.h>
#endif

int fe_resample_open(enum AVSampleFormat inSampleFmt,
                     enum AVSampleFormat outSampleFmt);

unsigned int fe_resample_do(AVFrame *inframe);


extern AVCodecContext *m_pCodecCtx;
extern enum AVSampleFormat m_pOutSampleFmt;
extern enum AVSampleFormat m_pInSampleFmt;


#ifndef __FFMPEGOLDAPI__

// Please choose to use libavresample.. people
// Compile it now.. but because macports doesn't
// Support both.. damn!
#ifdef __LIBAVRESAMPLE__
extern AVAudioResampleContext *m_pSwrCtx;
#else
extern SwrContext *m_pSwrCtx;
#endif
extern uint8_t *m_pOut;
#else
extern ReSampleContext *m_pSwrCtx;
extern short *m_pOut;
#endif

extern unsigned int m_pOutSize;

#endif
