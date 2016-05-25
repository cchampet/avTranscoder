#include "AudioCodec.hpp"

#include <cmath>
#include <cassert>

namespace avtranscoder
{

AudioCodec::AudioCodec(const ECodecType type, const std::string& codecName)
    : ICodec(type, codecName)
{
    loadCodecOptions(AV_OPT_FLAG_AUDIO_PARAM);
}

AudioCodec::AudioCodec(const ECodecType type, const AVCodecID codecId)
    : ICodec(type, codecId)
{
    loadCodecOptions(AV_OPT_FLAG_AUDIO_PARAM);
}

AudioCodec::AudioCodec(const ECodecType type, AVCodecContext& avCodecContext)
    : ICodec(type, avCodecContext)
{
    loadCodecOptions(AV_OPT_FLAG_AUDIO_PARAM);
}

AudioFrameDesc AudioCodec::getAudioFrameDesc() const
{
    assert(_avCodecContext != NULL);
    return AudioFrameDesc(_avCodecContext->sample_rate, _avCodecContext->channels, _avCodecContext->sample_fmt);
}

void AudioCodec::setAudioParameters(const AudioFrameDesc& audioFrameDesc)
{
    _avCodecContext->sample_rate = audioFrameDesc._sampleRate;
    _avCodecContext->channels = audioFrameDesc._nbChannels;
    _avCodecContext->sample_fmt = audioFrameDesc._sampleFormat;
}
}
