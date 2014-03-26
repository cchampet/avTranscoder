#ifndef _AV_TRANSCODER_VIDEO_STREAM_PROPERTIES_HPP_
#define _AV_TRANSCODER_VIDEO_STREAM_PROPERTIES_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/timecode.h>
}
#include <iostream>
#ifdef _MSC_VER
#include <float.h>
#define isnan _isnan
#define isinf(x) (!_finite(x))
#endif

namespace avtranscoder
{

namespace details
{

size_t decodeMpeg2SequenceHeader( const AVPacket* pkt, size_t offset = 0 )
{
	uint8_t* data = pkt->data;

	unsigned char d4 = data[offset++];
	unsigned char d5 = data[offset++];
	unsigned char d6 = data[offset++];
	unsigned char d7 = data[offset++];
	unsigned char d8 = data[offset++];
	unsigned char d9 = data[offset++];
	unsigned char d10 = data[offset++];
	unsigned char d11 = data[offset++];

	unsigned short width = ( d4 << 4 ) + ( ( d5 & 0xF0 ) >> 4 );
	unsigned short height = ( ( d5 & 0x0F ) << 8 ) + d6;

	unsigned int bitrate = ( d8 << 10 ) + ( d9 << 2 ) + ( ( d10 & 0xC0 ) >> 6 );

	std::cout << "video size: " << width << "x" << height << std::endl;
	std::cout << "bit rate: " << bitrate << std::endl;

	switch( d7 & 0xf0 >> 4 ) // frame rate
	{
		case 0 : std::cout << "forbidden aspect ratio" << std::endl; break;
		case 1 : std::cout << "aspect ratio = 1:1" << std::endl; break;
		case 2 : std::cout << "aspect ratio = 4:3" << std::endl; break;
		case 3 : std::cout << "aspect ratio = 16:9" << std::endl; break;
		case 4 : std::cout << "aspect ratio = 2.21:1" << std::endl; break;
		default : std::cout << "reserved aspect ratio" << std::endl; break;
	}

	switch( d7 & 0x0f ) // frame rate
	{
		case 0 : std::cout << "forbidden frame rate" << std::endl; break;
		case 1 : std::cout << "frame rate = 24000/1001 (23.976)" << std::endl; break;
		case 2 : std::cout << "frame rate = 24" << std::endl; break;
		case 3 : std::cout << "frame rate = 25" << std::endl; break;
		case 4 : std::cout << "frame rate = 30000/1001 (29.97)" << std::endl; break;
		case 5 : std::cout << "frame rate = 30" << std::endl; break;
		case 6 : std::cout << "frame rate = 50" << std::endl; break;
		case 7 : std::cout << "frame rate = 60000/1001 (59.94)" << std::endl; break;
		case 8 : std::cout << "frame rate = 60" << std::endl; break;
		default : std::cout << "reserved frame rate" << std::endl; break;
	}

	if( d11 && 0x2 )
	{
		offset += 64; // 64 byte of intra quantification matrix
		std::cout << "load intra quantiser matrix" << std::endl;

		if( d11 && 0x2 )
		{
			offset += 64; // 64 byte of non-intra quantification matrix
			std::cout << "load non-intra quantiser matrix" << std::endl;
		}
	}

	return offset;
}

size_t decodeMpeg2Extension( const AVPacket* pkt, size_t offset = 0 )
{
	uint8_t* data = pkt->data;

	unsigned char d4 = data[offset++];

	switch( (unsigned int) ( d4 >> 4 ) )
	{
		case 1 :
		{
			std::cout << "Sequence Extension" << std::endl;
			offset += 5;
			break;
		}
		case 2 :
		{
			std::cout << "Sequence Display Extension" << std::endl;
			if( d4 && 0x01 )
				offset += 7;
			else
				offset += 4;
			break;
		}
		case 8 :
		{
			std::cout << "Picture Coding Extension" << std::endl;
			unsigned char d5 = data[offset++];
			unsigned char d6 = data[offset++];
			unsigned char d7 = data[offset++];
			unsigned char d8 = data[offset++];

			if( ( d8 && 0x40 ) != 0)
			{
				unsigned char d9 = data[offset++];
				unsigned char d10 = data[offset++];
			}
			break;
		}
		default:
		{
			std::cout << "unknown extension " << ( ( d4 && 0xf0 ) ) << std::endl;
		}
	}

	return offset;
}

size_t decodeMpeg2GOP( const AVPacket* pkt, size_t offset = 0 )
{
	uint8_t* data = pkt->data;

	unsigned char d4 = data[offset++];
	unsigned char d5 = data[offset++];
	unsigned char d6 = data[offset++];
	unsigned char d7 = data[offset++];

	size_t hour   = 0;
	size_t minute = 0;
	size_t second = 0;
	size_t frame  = 0;

	// hour = (unsigned int)( d4 && 0x7C ) >> 2;
	// minute = (unsigned int)( d4 && 0x7C ) >> 2;
	// second = (unsigned int)( d4 && 0x7C ) >> 2;
	// frame = (unsigned int)( d4 && 0x7C ) >> 2;

	// std::cout << hour << ":" << minute << ":" << second << ":" << frame;

	return offset;
}

void decodeMpeg2Header( const AVPacket* pkt, size_t offset = 0 )
{
	std::cout << "----------------------------"<< std::endl;
	uint8_t* data = pkt->data;

	bool decodingNextHeaderElement = true;

	while( decodingNextHeaderElement )
	{
		unsigned char d0 = data[offset++];
		unsigned char d1 = data[offset++];
		unsigned char d2 = data[offset++];
		unsigned char d3 = data[offset++];
		//std::cout << offset << " | start code " << (int)d0 << " - " << (int)d1 << " - " << (int)d2 << " - " << (unsigned int)d3 << std::endl;

		if( ( (int)d0 != 0 ) || ( (int)d1 != 0 ) || ( (int)d2 != 1 ) )
		{
			decodingNextHeaderElement = false;
			continue;
		}


		switch( d3 )
		{
			case 0x00: std::cout << "start code prefix for Picture" << std::endl; break;
			case 0xB0: std::cout << "start code prefix : reserved" << std::endl; break;
			case 0xB1: std::cout << "start code prefix : reserved" << std::endl; break;
			case 0xB2: std::cout << "start code prefix for User data" << std::endl; break;
			case 0xB3: std::cout << "start code prefix for Sequence Header" << std::endl; offset = decodeMpeg2SequenceHeader( pkt, offset ); break;
			case 0xB4: std::cout << "start code prefix for Sequence Error" << std::endl; break;
			case 0xB5: std::cout << "start code prefix for Extension" << std::endl; offset = decodeMpeg2Extension( pkt, offset ); break;
			case 0xB6: std::cout << "start code prefix for Reserved" << std::endl; break;
			case 0xB7: std::cout << "start code prefix for Sequence End" << std::endl; break;
			case 0xB8: std::cout << "start code prefix for Group Of Picture" << std::endl; offset = decodeMpeg2GOP( pkt, offset ); break;
			default:
			{
				if( d3 < 0xB0 )
					std::cout << "start code prefix for slice" << std::endl;
				else
					std::cout << "UNKNOWN start code prefix" << std::endl;
				break;
			}
		}
	}
}

void getGopProperties( VideoProperties& vp, AVFormatContext* formatContext, AVCodecContext* codecContext, AVCodec* codec, const int index )
{
	AVPacket pkt;

#if LIBAVCODEC_VERSION_MAJOR > 54
	AVFrame* frame = av_frame_alloc();
#else
	AVFrame* frame = avcodec_alloc_frame();
#endif

	av_init_packet( &pkt );
	avcodec_open2( codecContext, codec, NULL );

	int count = 0;
	int gotFrame = 0;
	
	while( ! av_read_frame( formatContext, &pkt ) )
	{
		if( pkt.stream_index == index )
		{
			decodeMpeg2Header( &pkt );

			avcodec_decode_video2( codecContext, frame, &gotFrame, &pkt );
			if( gotFrame )
			{
				vp.gopStructure.push_back( std::pair<char, bool>( av_get_picture_type_char( frame->pict_type ), frame->key_frame ) );
				vp.isInterlaced  = frame->interlaced_frame;
				vp.topFieldFirst = frame->top_field_first;
				++count;
			}
		}

		av_free_packet( &pkt );
		
		if( codecContext->gop_size == count )
		{
			break;
		}
	}
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_free( &frame );
#else
 #if LIBAVCODEC_VERSION_MAJOR > 53
	avcodec_free_frame( &frame );
 #else
	av_free( frame );
 #endif
#endif
}

}

VideoProperties videoStreamInfo( AVFormatContext* formatContext, const size_t index )
{
	VideoProperties vp;
	AVCodecContext* codec_context = formatContext->streams[index]->codec;
	
	codec_context->skip_frame = AVDISCARD_NONE;

	vp.streamId         = index;

	vp.codecName        = codec_context->codec_name;
	vp.codecLongName    = codec_context->codec_name;
	vp.codecId          = codec_context->codec_id;
	vp.bitRate          = codec_context->bit_rate;
	vp.maxBitRate       = codec_context->rc_max_rate;
	vp.minBitRate       = codec_context->rc_min_rate;
	vp.isInterlaced     = false;

	vp.ticksPerFrame    = codec_context->ticks_per_frame,
	vp.width            = codec_context->width,
	vp.height           = codec_context->height,
	vp.gopSize          = codec_context->gop_size,
	vp.dtgActiveFormat  = codec_context->dtg_active_format,
	vp.referencesFrames = codec_context->refs,
	vp.profile          = codec_context->profile,
	vp.level            = codec_context->level;

	vp.timeBase.num     = codec_context->time_base.num;
	vp.timeBase.den     = codec_context->time_base.den;
	vp.sar.num          = codec_context->sample_aspect_ratio.num;
	vp.sar.den          = codec_context->sample_aspect_ratio.den;
	

	char tcbuf[AV_TIMECODE_STR_SIZE];
	av_timecode_make_mpeg_tc_string( tcbuf, codec_context->timecode_frame_start );
	std::string videoTimecode( tcbuf );
	vp.startTimecode    = videoTimecode;

	int darNum, darDen;
	av_reduce( &darNum, &darDen,
			   codec_context->width * codec_context->sample_aspect_ratio.num,
			   codec_context->height * codec_context->sample_aspect_ratio.den,
			   1024 * 1024);
	vp.dar.num = darNum;
	vp.dar.den = darDen;

	vp.fps = 1.0 * codec_context->time_base.den / ( codec_context->time_base.num * codec_context->ticks_per_frame );
	
	if( isinf( vp.fps ) )
		vp.fps = 0.0;

	vp.hasBFrames           = (bool) codec_context->has_b_frames;

	
	vp.colorspace           = "";
	vp.colorTransfert       = "";
	vp.colorRange           = "";
	vp.colorPrimaries       = "";
	vp.chromaSampleLocation = "";
	vp.fieldOrder           = "";
	
	switch( codec_context->color_trc )
	{
		case AVCOL_TRC_BT709:        vp.colorTransfert = "Rec 709 / ITU-R BT1361"; break;
		case AVCOL_TRC_UNSPECIFIED:  vp.colorTransfert = "unspecified"; break;
		case AVCOL_TRC_GAMMA22:      vp.colorTransfert = "Gamma 2.2"; break;
		case AVCOL_TRC_GAMMA28:      vp.colorTransfert = "Gamma 2.8"; break;
		case AVCOL_TRC_SMPTE240M:    vp.colorTransfert = "Smpte 240M"; break;
#if LIBAVCODEC_VERSION_MAJOR > 54
 #ifdef AVCOL_TRC_SMPTE170M
		case AVCOL_TRC_SMPTE170M:    vp.colorTransfert = "Rec 601 / ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC"; break;
 #endif
 #ifdef AVCOL_TRC_LINEAR
		case AVCOL_TRC_LINEAR:       vp.colorTransfert = "Linear transfer characteristics"; break;
 #endif
 #ifdef AVCOL_TRC_LOG
		case AVCOL_TRC_LOG:          vp.colorTransfert = "Logarithmic transfer characteristic (100:1 range)"; break;
 #endif
 #ifdef AVCOL_TRC_LOG_SQRT
		case AVCOL_TRC_LOG_SQRT:     vp.colorTransfert = "Logarithmic transfer characteristic (100 * Sqrt( 10 ) : 1 range)"; break;
 #endif
 #ifdef AVCOL_TRC_IEC61966_2_4
		case AVCOL_TRC_IEC61966_2_4: vp.colorTransfert = "IEC 61966-2-4"; break;
 #endif
 #ifdef AVCOL_TRC_BT1361_ECG
		case AVCOL_TRC_BT1361_ECG:   vp.colorTransfert = "ITU-R BT1361 Extended Colour Gamut"; break;
 #endif
 #ifdef AVCOL_TRC_IEC61966_2_1
		case AVCOL_TRC_IEC61966_2_1: vp.colorTransfert = "IEC 61966-2-1 (sRGB or sYCC)"; break;
 #endif
 #ifdef AVCOL_TRC_BT2020_10
		case AVCOL_TRC_BT2020_10:    vp.colorTransfert = "ITU-R BT2020 for 10 bit system"; break;
 #endif
 #ifdef AVCOL_TRC_BT2020_12
		case AVCOL_TRC_BT2020_12:    vp.colorTransfert = "ITU-R BT2020 for 12 bit system"; break;	
 #endif
#endif
		case AVCOL_TRC_NB:           vp.colorTransfert = "Not ABI"; break;
	}
	switch( codec_context->colorspace )
	{
		case AVCOL_SPC_RGB:         vp.colorspace = "RGB"; break;
		case AVCOL_SPC_BT709:       vp.colorspace = "Rec 709"; break;
		case AVCOL_SPC_UNSPECIFIED: vp.colorspace = "unspecified"; break;
		case AVCOL_SPC_FCC:         vp.colorspace = "Four CC"; break;
		case AVCOL_SPC_BT470BG:     vp.colorspace = "BT470 (PAL - 625)"; break;
		case AVCOL_SPC_SMPTE170M:   vp.colorspace = "Smpte 170M (NTSC)"; break;
		case AVCOL_SPC_SMPTE240M:   vp.colorspace = "Smpte 240M"; break;
#if LIBAVCODEC_VERSION_MAJOR > 53
		case AVCOL_SPC_YCOCG:       vp.colorspace = "Y Co Cg"; break;
#else
		case AVCOL_SPC_YCGCO:       vp.colorspace = "Y Cg Co"; break;
#endif
#if LIBAVCODEC_VERSION_MAJOR > 54
 #ifdef AVCOL_TRC_BT2020_12
		case AVCOL_SPC_BT2020_NCL:  vp.colorspace = "ITU-R BT2020 non-constant luminance system"; break;
 #endif
 #ifdef AVCOL_TRC_BT2020_CL
		case AVCOL_SPC_BT2020_CL:   vp.colorspace = "ITU-R BT2020 constant luminance system"; break;
 #endif
#endif
		case AVCOL_SPC_NB:          vp.colorspace = "Not ABI"; break;
	}
	switch( codec_context->color_range )
	{
		case AVCOL_RANGE_UNSPECIFIED: vp.colorRange = "unspecified"; break;
		case AVCOL_RANGE_MPEG:        vp.colorRange = "Head"; break;
		case AVCOL_RANGE_JPEG:        vp.colorRange = "Full"; break;
		case AVCOL_RANGE_NB:          vp.colorRange = "Not ABI"; break;
	}
	switch( codec_context->color_primaries )
	{
		case AVCOL_PRI_BT709:       vp.colorPrimaries = "Rec 709"; break;
		case AVCOL_PRI_UNSPECIFIED: vp.colorPrimaries = "unspecified"; break;
		case AVCOL_PRI_BT470M:      vp.colorPrimaries = "BT 470M"; break;
		case AVCOL_PRI_BT470BG:     vp.colorPrimaries = "Rec 601 (PAL & SECAM)"; break;
		case AVCOL_PRI_SMPTE170M:   vp.colorPrimaries = "Rec 601 (NTSC)"; break;
		case AVCOL_PRI_SMPTE240M:   vp.colorPrimaries = "Smpte 240 (NTSC)"; break;
		case AVCOL_PRI_FILM:        vp.colorPrimaries = "Film"; break;
#if LIBAVCODEC_VERSION_MAJOR > 54
 #ifdef AVCOL_TRC_BT2020_CL
		case AVCOL_PRI_BT2020:      vp.colorPrimaries = "ITU-R BT2020"; break;
 #endif
#endif
		case AVCOL_PRI_NB:          vp.colorPrimaries = "Not ABI"; break;
	}
	switch( codec_context->chroma_sample_location )
	{
		case AVCHROMA_LOC_UNSPECIFIED: vp.chromaSampleLocation = "unspecified"; break;
		case AVCHROMA_LOC_LEFT:        vp.chromaSampleLocation = "left (mpeg2/4, h264 default)"; break;
		case AVCHROMA_LOC_CENTER:      vp.chromaSampleLocation = "center (mpeg1, jpeg, h263)"; break;
		case AVCHROMA_LOC_TOPLEFT:     vp.chromaSampleLocation = "top left"; break;
		case AVCHROMA_LOC_TOP:         vp.chromaSampleLocation = "top"; break;
		case AVCHROMA_LOC_BOTTOMLEFT:  vp.chromaSampleLocation = "bottom left"; break;
		case AVCHROMA_LOC_BOTTOM:      vp.chromaSampleLocation = "bottom"; break;
		case AVCHROMA_LOC_NB:          vp.chromaSampleLocation = "Not ABI"; break;
	}
	switch( codec_context->field_order )
	{
		case AV_FIELD_UNKNOWN:     vp.fieldOrder = "unknown"; break;
		case AV_FIELD_PROGRESSIVE: vp.fieldOrder = "progressive"; break;
		case AV_FIELD_TT:          vp.fieldOrder = "top top"; break;
		case AV_FIELD_BB:          vp.fieldOrder = "bottom bottom"; break;
		case AV_FIELD_TB:          vp.fieldOrder = "top bottom"; break;
		case AV_FIELD_BT:          vp.fieldOrder = "bottom top"; break;
	}
#if LIBAVUTIL_VERSION_MAJOR > 51
	const AVPixFmtDescriptor* pixFmt = av_pix_fmt_desc_get( codec_context->pix_fmt );
#else
	const AVPixFmtDescriptor* pixFmt = NULL;
	if( codec_context->pix_fmt > 0 && codec_context->pix_fmt < PIX_FMT_NB )
		pixFmt = &av_pix_fmt_descriptors[ codec_context->pix_fmt ];
#endif

	if( pixFmt != NULL )
	{
		vp.pixelName            = pixFmt->name;
		vp.componentsCount      = pixFmt->nb_components;
		vp.chromaWidth          = pixFmt->log2_chroma_w;
		vp.chromaHeight         = pixFmt->log2_chroma_h;
		vp.endianess            = ( pixFmt->flags & PIX_FMT_BE ) ? "big" : "little";
		vp.indexedColors        = (bool) pixFmt->flags & PIX_FMT_PAL;
		vp.bitWisePacked        = (bool) pixFmt->flags & PIX_FMT_BITSTREAM;
		vp.hardwareAcceleration = (bool) pixFmt->flags & PIX_FMT_HWACCEL;
		vp.notFirstPlane        = (bool) pixFmt->flags & PIX_FMT_PLANAR;
		vp.rgbPixelData         = (bool) pixFmt->flags & PIX_FMT_RGB;
#if LIBAVCODEC_VERSION_MAJOR > 53
		vp.pseudoPaletted       = (bool) pixFmt->flags & PIX_FMT_PSEUDOPAL;
		vp.asAlpha              = (bool) pixFmt->flags & PIX_FMT_ALPHA;
#else
#endif

		for( size_t channel = 0; channel < (size_t)pixFmt->nb_components; ++channel )
		{
			Channel c;
			c.id           = channel;
			c.chromaHeight = (size_t)pixFmt->comp[channel].plane;
			c.bitStep      = (size_t)pixFmt->comp[channel].step_minus1;
			vp.channels.push_back( c );
		}
	}

	AVCodec* codec = NULL;
	if( ( codec = avcodec_find_decoder( codec_context->codec_id ) ) != NULL )
	{
		if( codec->capabilities & CODEC_CAP_TRUNCATED )
			codec_context->flags|= CODEC_FLAG_TRUNCATED;

		vp.codecName     = codec->name;
		vp.codecLongName = codec->long_name;

		if( codec_context->profile == -99 )
		{
			const char* profile;
			if( ( profile = av_get_profile_name( codec, codec_context->profile ) ) != NULL )
				vp.profileName = profile;
		}
		
		if( codec_context->width && codec_context->height )
		{
			details::getGopProperties( vp, formatContext, codec_context, codec, index);
		}
	}

	return vp;
}

}

#endif