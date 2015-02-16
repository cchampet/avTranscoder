#include "VideoFrame.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>
#include <stdlib.h>
#include <cmath>

namespace avtranscoder
{

VideoFrameDesc::VideoFrameDesc( const AVCodecContext* context )
	: _width( context->width )
	, _height( context->height )
	, _pixelFormat( context->pix_fmt )
	, _timeBase( context->time_base )
	, _ticksPerFrame( context->ticks_per_frame )
{
}

VideoFrameDesc::VideoFrameDesc( const size_t width, const size_t height, const AVPixelFormat pixelFormat )
	: _width( width )
	, _height( height )
	, _pixelFormat( pixelFormat )
	, _timeBase()
	, _ticksPerFrame( 1.0 )
{
	_timeBase.num = 0;
	_timeBase.den = 0;
}
VideoFrameDesc::VideoFrameDesc( const size_t width, const size_t height, const std::string& pixelFormat )
	: _width( width )
	, _height( height )
	, _pixelFormat( av_get_pix_fmt( pixelFormat.c_str() ) )
	, _timeBase()
	, _ticksPerFrame( 1.0 )
{
	_timeBase.num = 0;
	_timeBase.den = 0;
}

std::string VideoFrameDesc::getPixelFormatName() const
{
	const char* formatName = av_get_pix_fmt_name( _pixelFormat );
	return formatName ? std::string( formatName ) : "unknown pixel format";
}

double VideoFrameDesc::getFps() const
{
	double fps = 1.0 * _timeBase.den / ( _timeBase.num * _ticksPerFrame );
	if( ! std::isinf( fps ) )
		return fps;
	return 1.0;
}

size_t VideoFrameDesc::getDataSize() const
{
	if( _pixelFormat == AV_PIX_FMT_NONE )
		throw std::runtime_error( "incorrect pixel format" );

	size_t size = avpicture_get_size( _pixelFormat, _width, _height );
	if( size == 0 )
		throw std::runtime_error( "unable to determine image buffer size" );

	return size;
}

void VideoFrameDesc::setPixelFormat( const std::string& pixelFormat )
{
	_pixelFormat = av_get_pix_fmt( pixelFormat.c_str() );
}

void VideoFrameDesc::setFps( const double fps, const double ticksPerFrame )
{
	_timeBase.num = 1;
	_timeBase.den = fps;
	_ticksPerFrame = ticksPerFrame;
}

void VideoFrameDesc::setParameters( const ProfileLoader::Profile& profile )
{
	// width
	if( profile.count( constants::avProfileWidth ) )
		setWidth( atoi( profile.find( constants::avProfileWidth )->second.c_str() ) );
	// height
	if( profile.count( constants::avProfileHeight ) )
		setHeight( atoi( profile.find( constants::avProfileHeight )->second.c_str() ) );
	// pixel format	
	if( profile.count( constants::avProfilePixelFormat ) )
		setPixelFormat( profile.find( constants::avProfilePixelFormat )->second );
	// fps
	if( profile.count( constants::avProfileFrameRate ) )
		setFps( atof( profile.find( constants::avProfileFrameRate )->second.c_str() ) );
}

}


