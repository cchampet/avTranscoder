#include "VideoCodec.hpp"

#include <cassert>

namespace avtranscoder
{

VideoCodec::VideoCodec( const ECodecType type, const std::string& codecName )
	: ICodec( type, codecName )
{
}

VideoCodec::VideoCodec( const ECodecType type, const AVCodecID codecId )
	: ICodec( type, codecId )
{
}

VideoCodec::VideoCodec( const ECodecType type, AVCodecContext& avCodecContext )
	: ICodec( type, avCodecContext )
{
}

VideoFrameDesc VideoCodec::getVideoFrameDesc() const
{
	assert( _avCodecContext != NULL );
	VideoFrameDesc videoFrameDesc( _avCodecContext );
	return videoFrameDesc;
}

void VideoCodec::setImageParameters( const VideoFrameDesc& videoFrameDesc )
{
	_avCodecContext->width = videoFrameDesc.getWidth();
	_avCodecContext->height = videoFrameDesc.getHeight();
	_avCodecContext->pix_fmt = videoFrameDesc.getPixelFormat();
	_avCodecContext->time_base.num = videoFrameDesc.getTimeBase().num;
	_avCodecContext->time_base.den = videoFrameDesc.getTimeBase().den;
	_avCodecContext->ticks_per_frame = videoFrameDesc.getTicksPerFrame();
}

}
