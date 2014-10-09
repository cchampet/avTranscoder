%module AvTranscoder

%module(directors="1") AvTranscoder

%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"

%include "AvTranscoder/swig/avException.i"
%include "AvTranscoder/swig/avExport.i"

%{
#include <AvTranscoder/common.hpp>

#include <AvTranscoder/Profile.hpp>

#include <AvTranscoder/essenceStructures/Pixel.hpp>
#include <AvTranscoder/essenceStructures/Frame.hpp>
#include <AvTranscoder/essenceStructures/VideoFrame.hpp>
#include <AvTranscoder/essenceStructures/AudioFrame.hpp>

#include <AvTranscoder/codedStructures/CodedDesc.hpp>
#include <AvTranscoder/codedStructures/VideoDesc.hpp>
#include <AvTranscoder/codedStructures/AudioDesc.hpp>
#include <AvTranscoder/codedStructures/DataDesc.hpp>
#include <AvTranscoder/codedStructures/DataStream.hpp>

#include <AvTranscoder/mediaProperty/mediaProperty.hpp>

#include <AvTranscoder/Profile.hpp>

#include <AvTranscoder/codedStream/IOutputStream.hpp>
#include <AvTranscoder/codedStream/AvOutputStream.hpp>

#include <AvTranscoder/codedStream/IInputStream.hpp>
#include <AvTranscoder/codedStream/AvInputStream.hpp>

#include <AvTranscoder/essenceStream/IOutputEssence.hpp>
#include <AvTranscoder/essenceStream/AvOutputAudio.hpp>
#include <AvTranscoder/essenceStream/AvOutputVideo.hpp>

#include <AvTranscoder/essenceStream/IInputEssence.hpp>
#include <AvTranscoder/essenceStream/AvInputAudio.hpp>
#include <AvTranscoder/essenceStream/AvInputVideo.hpp>

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/file/OutputFile.hpp>

#include <AvTranscoder/transcoder/StreamTranscoder.hpp>
#include <AvTranscoder/transcoder/Transcoder.hpp>
%}

namespace std {	
%template(IntPair)         pair< size_t, size_t >;
%template(VideoVector)     vector< avtranscoder::VideoProperties >;
%template(AudioVector)     vector< avtranscoder::AudioProperties >;
%template(MetadataPair)    pair< string, string >;
%template(MetadatasVector) vector< pair< string, string > >;
%template(GopPair)         pair< char, bool >;
%template(GopVector)       vector< pair< char, bool > >;
%template(ChannelVector)   vector< avtranscoder::Channel >;
%template(ProfileMap)      map< string, string >;
%template(ProfilesVector)  vector< map< string, string > >;
}

%include <AvTranscoder/common.hpp>

%include "AvTranscoder/progress/progress.i"

%include <AvTranscoder/Profile.hpp>

%include <AvTranscoder/essenceStructures/Pixel.hpp>
%include <AvTranscoder/essenceStructures/Frame.hpp>
%include <AvTranscoder/essenceStructures/VideoFrame.hpp>
%include <AvTranscoder/essenceStructures/AudioFrame.hpp>

%include <AvTranscoder/codedStructures/CodedDesc.hpp>
%include <AvTranscoder/codedStructures/VideoDesc.hpp>
%include <AvTranscoder/codedStructures/AudioDesc.hpp>
%include <AvTranscoder/codedStructures/DataDesc.hpp>
%include <AvTranscoder/codedStructures/DataStream.hpp>


%include <AvTranscoder/mediaProperty/mediaProperty.hpp>

%include <AvTranscoder/Profile.hpp>

%include <AvTranscoder/codedStream/IOutputStream.hpp>
%include <AvTranscoder/codedStream/AvOutputStream.hpp>

%include <AvTranscoder/codedStream/IInputStream.hpp>
%include <AvTranscoder/codedStream/AvInputStream.hpp>

%include <AvTranscoder/essenceStream/IOutputEssence.hpp>
%include <AvTranscoder/essenceStream/AvOutputAudio.hpp>
%include <AvTranscoder/essenceStream/AvOutputVideo.hpp>

%include <AvTranscoder/essenceStream/IInputEssence.hpp>
%include <AvTranscoder/essenceStream/AvInputAudio.hpp>
%include <AvTranscoder/essenceStream/AvInputVideo.hpp>

%include <AvTranscoder/file/InputFile.hpp>
%include <AvTranscoder/file/OutputFile.hpp>

%include <AvTranscoder/transcoder/StreamTranscoder.hpp>
%include <AvTranscoder/transcoder/Transcoder.hpp>
