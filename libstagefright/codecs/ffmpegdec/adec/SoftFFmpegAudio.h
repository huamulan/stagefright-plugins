/*
 * Copyright 2012 Michael Chen <omxcodec@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SOFT_FFMPEGAUDIO_H_

#define SOFT_FFMPEGAUDIO_H_

#include "SimpleSoftOMXComponent.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#include <inttypes.h>
#include <math.h>
#include <limits.h> /* INT_MAX */

#include "libavutil/avstring.h"
#include "libavutil/colorspace.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/intreadwrite.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavcodec/audioconvert.h"
#include "libavutil/opt.h"
#include "libavutil/internal.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"

#include "cmdutils.h"

#ifdef __cplusplus
}
#endif

namespace android {

struct SoftFFmpegAudio : public SimpleSoftOMXComponent {
    SoftFFmpegAudio(const char *name,
            const OMX_CALLBACKTYPE *callbacks,
            OMX_PTR appData,
            OMX_COMPONENTTYPE **component);

protected:
    virtual ~SoftFFmpegAudio();

    virtual OMX_ERRORTYPE internalGetParameter(
            OMX_INDEXTYPE index, OMX_PTR params);

    virtual OMX_ERRORTYPE internalSetParameter(
            OMX_INDEXTYPE index, const OMX_PTR params);

    virtual void onQueueFilled(OMX_U32 portIndex);
    virtual void onPortFlushCompleted(OMX_U32 portIndex);
    virtual void onPortEnableCompleted(OMX_U32 portIndex, bool enabled);

private:
    enum {
        kNumBuffers = 4,
        kOutputBufferSize = 4608 * 2
    };

    enum {
        MODE_MPEG,
	MODE_MPEGL1,
	MODE_MPEGL2,
        MODE_AAC,
        MODE_AC3,
        MODE_APE,
        MODE_WMA,
        MODE_DTS,
        MODE_RA
    } mMode;

    enum {
        kPortIndexInput  = 0,
        kPortIndexOutput = 1,
    };

    AVCodecContext *mCtx;
    struct SwrContext *mSwrCtx;

    bool mCodecOpened;
    bool mExtradataReady;
    bool mIgnoreExtradata;
    bool mFlushComplete;
    bool mSignalledError;
    bool mReceivedEOS;

    AVFrame *mFrame;

    int64_t mAnchorTimeUs;
    int64_t mNumFramesOutput;
    int32_t mInputBufferSize;

    uint8_t mSilenceBuffer[kOutputBufferSize];
    uint8_t *mPAudioBuffer;
    int32_t mAudioBufferSize;

    int32_t mNumChannels;
    int32_t mSamplingRate;
    /* some audio codec need bit rate when init, e.g. wma, and should be > 0 */
    int32_t mBitRate;
    int32_t mBlockAlign;
    AVSampleFormat mSamplingFmt;
    bool mAudioConfigChanged;

    enum AVSampleFormat mAudioSrcFmt;
    enum AVSampleFormat mAudioTgtFmt;
    int mAudioSrcChannels;
    int mAudioTgtChannels;
    int64_t mAudioSrcChannelLayout;
    int64_t mAudioTgtChannelLayout;
    int mAudioSrcFreq;
    int mAudioTgtFreq;

    enum {
        NONE,
        AWAITING_DISABLED,
        AWAITING_ENABLED
    } mOutputPortSettingsChange;

    void setAVCtxToDefault(AVCodecContext *avctx, const AVCodec *codec);

    void initPorts();
    status_t initDecoder();
    void deInitDecoder();

    DISALLOW_EVIL_CONSTRUCTORS(SoftFFmpegAudio);
};

}  // namespace android

#endif  // SOFT_FFMPEGAUDIO_H_


