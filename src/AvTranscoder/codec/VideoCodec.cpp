#include "VideoCodec.hpp"

#include <cmath>
#include <cassert>

namespace avtranscoder
{

VideoCodec::VideoCodec(const ECodecType type, const std::string& codecName)
    : ICodec(type, codecName)
{
    loadCodecOptions(AV_OPT_FLAG_VIDEO_PARAM);
}

VideoCodec::VideoCodec(const ECodecType type, const AVCodecID codecId)
    : ICodec(type, codecId)
{
    loadCodecOptions(AV_OPT_FLAG_VIDEO_PARAM);
}

VideoCodec::VideoCodec(const ECodecType type, AVCodecContext& avCodecContext)
    : ICodec(type, avCodecContext)
{
    loadCodecOptions(AV_OPT_FLAG_VIDEO_PARAM);
}

VideoFrameDesc VideoCodec::getVideoFrameDesc() const
{
    assert(_avCodecContext != NULL);
    VideoFrameDesc videoFrameDesc(_avCodecContext->width, _avCodecContext->height, _avCodecContext->pix_fmt);
    double fps = 1.0 * _avCodecContext->time_base.den / (_avCodecContext->time_base.num * _avCodecContext->ticks_per_frame);
    if(!std::isinf(fps))
        videoFrameDesc._fps = fps;
    return videoFrameDesc;
}

void VideoCodec::setImageParameters(const VideoFrameDesc& videoFrameDesc)
{
    _avCodecContext->width = videoFrameDesc._width;
    _avCodecContext->height = videoFrameDesc._height;
    _avCodecContext->pix_fmt = videoFrameDesc._pixelFormat;
    _avCodecContext->time_base.num = 1;
    _avCodecContext->time_base.den = videoFrameDesc._fps;
    _avCodecContext->ticks_per_frame = 1;
}
}
