#include "InputFile.hpp"

#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/mediaProperty/VideoProperties.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>
#include <AvTranscoder/mediaProperty/DataProperties.hpp>
#include <AvTranscoder/mediaProperty/SubtitleProperties.hpp>
#include <AvTranscoder/mediaProperty/AttachementProperties.hpp>
#include <AvTranscoder/mediaProperty/UnknownProperties.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>
#include <sstream>

namespace avtranscoder
{

InputFile::InputFile( const std::string& filename )
	: _formatContext( filename, AV_OPT_FLAG_DECODING_PARAM )
	, _properties( _formatContext )
	, _filename( filename )
	, _inputStreams()
{
	_formatContext.findStreamInfo();

	// Create streams
	for( size_t streamIndex = 0; streamIndex < _formatContext.getNbStreams(); ++streamIndex )
	{
		_inputStreams.push_back( new InputStream( *this, streamIndex ) );
	}
}

InputFile::~InputFile()
{
	for( std::vector< InputStream* >::iterator it = _inputStreams.begin(); it != _inputStreams.end(); ++it )
	{
		delete (*it);
	}
}

void InputFile::analyse( IProgress& progress, const EAnalyseLevel level )
{
	_properties.extractStreamProperties( progress, level );
}

FileProperties InputFile::analyseFile( const std::string& filename, IProgress& progress, const EAnalyseLevel level )
{
	InputFile file( filename );
	file.analyse( progress, level );
	return file.getProperties();
}

bool InputFile::readNextPacket( CodedData& data, const size_t streamIndex )
{
	bool nextPacketFound = false;
	while( ! nextPacketFound )
	{
		int ret = av_read_frame( &_formatContext.getAVFormatContext(), &data.getAVPacket() );
		if( ret < 0 ) // error or end of file
		{
			return false;
		}

		// if the packet stream is the expected one
		// return the packet data
		int packetStreamIndex = data.getAVPacket().stream_index;
		if( packetStreamIndex == (int)streamIndex )
		{
			nextPacketFound = true;
		}
		// else add the packet data to the stream cache
		else
		{
			_inputStreams.at( packetStreamIndex )->addPacket( data.getAVPacket() );
			data.clear();
		}
	}
	return true;
}

void InputFile::seekAtFrame( const size_t frame, const int flag, const int streamIndex )
{
	// get time base
	uint64_t timeBase = 0;
	double fps = 1;
	if( streamIndex == -1 )
	{
		// file time base
		timeBase = AV_TIME_BASE;
	}
	else
	{
		// stream time base
		const AVRational streamTimeScale = _formatContext.getAVStream(streamIndex).time_base;
		timeBase = streamTimeScale.den / streamTimeScale.num;

		const StreamProperties& streamProperties = _properties.getStreamPropertiesWithIndex( streamIndex );
		if( streamProperties.getStreamType() == AVMEDIA_TYPE_VIDEO )
			fps = dynamic_cast<const VideoProperties&>( streamProperties ).getFps();
	}
	// seek at position
	const uint64_t position = frame / fps * timeBase;
	return _formatContext.seek( position, flag, streamIndex );
}

void InputFile::seekAtTime( const double time, const int flag, const int streamIndex )
{
	const uint64_t position = time * AV_TIME_BASE;
	return _formatContext.seek( position, flag, streamIndex );
}

void InputFile::activateStream( const size_t streamIndex, bool activate )
{
	getStream( streamIndex ).activate( activate );
}

InputStream& InputFile::getStream( size_t index )
{
	try
	{
		return *_inputStreams.at( index );
	}
	catch( const std::out_of_range& e )
	{
		std::stringstream msg;
		msg << getFilename();
		msg << " has no stream at index ";
		msg << index;
		throw std::runtime_error( msg.str() );
	}
}


std::string InputFile::getFormatName() const
{
	if( _formatContext.getAVInputFormat().name == NULL )
	{
		LOG_WARN("Unknown demuxer format name of '" << _filename << "'.")
		return "";
	}
	return std::string(_formatContext.getAVInputFormat().name);
}

std::string InputFile::getFormatLongName() const
{
	if( _formatContext.getAVInputFormat().long_name == NULL )
	{
		LOG_WARN("Unknown demuxer format long name of '" << _filename << "'.")
		return "";
	}
	return std::string(_formatContext.getAVInputFormat().long_name);
}

std::string InputFile::getFormatMimeType() const
{
#if LIBAVFORMAT_VERSION_MAJOR <= 55
	LOG_WARN("Cannot get mime type format of '" << _filename << "' because your libavformat library has a major version <= 55.")
	return "not available";
#else
	if( _formatContext.getAVInputFormat().mime_type == NULL )
	{
		LOG_WARN("Unknown demuxer format mime type of '" << _filename << "'.")
		return "";
	}
	return std::string(_formatContext.getAVInputFormat().mime_type);
#endif
}

void InputFile::setupUnwrapping( const ProfileLoader::Profile& profile )
{
	LOG_DEBUG( "Set profile of input file with:\n" << profile )

	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType )
			continue;
		
		try
		{
			Option& formatOption = _formatContext.getOption( (*it).first );
			formatOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			LOG_WARN( "InputFile - can't set option " << (*it).first <<  " to " << (*it).second << ": " << e.what() )
		}
	}
}

}