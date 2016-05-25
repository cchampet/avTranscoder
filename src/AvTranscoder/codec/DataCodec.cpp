#include "DataCodec.hpp"

namespace avtranscoder
{

DataCodec::DataCodec(const ECodecType type, const std::string& codecName)
    : ICodec(type, codecName)
{
    loadCodecOptions();
}

DataCodec::DataCodec(const ECodecType type, const AVCodecID codecId)
    : ICodec(type, codecId)
{
    loadCodecOptions();
}

DataCodec::DataCodec(const ECodecType type, AVCodecContext& avCodecContext)
    : ICodec(type, avCodecContext)
{
    loadCodecOptions();
}
}
