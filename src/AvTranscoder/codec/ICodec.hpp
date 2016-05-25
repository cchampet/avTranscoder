#ifndef _AV_TRANSCODER_CODEC_ICODEC_HPP_
#define _AV_TRANSCODER_CODEC_ICODEC_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/Option.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <string>

namespace avtranscoder
{

/**
 * @brief Define if a codec is for encoding or decoding.
 */
enum ECodecType
{
    eCodecTypeEncoder,
    eCodecTypeDecoder
};

class AvExport ICodec
{
private:
    ICodec(const ICodec& iCodec);
    ICodec& operator=(const ICodec& iCodec);

public:
    ICodec(const ECodecType type, const std::string& codecName);
    ICodec(const ECodecType type, const AVCodecID codecId);
    ICodec(const ECodecType type, AVCodecContext& avCodecContext);

    virtual ~ICodec() = 0;

    /**
     * @brief Initialize the codec context.
     * @see closeCodec
     */
    void openCodec();

    /**
     * @brief Close the codec context.
     * @note free all the data associated with it (but not the codec context itself).
     */
    void closeCodec();

    AVCodecID getCodecId() const;
    std::string getCodecName() const;
    std::string getCodecLongName() const;
    ECodecType getCodecType() const { return _type; }
    int getLatency() const;

    /**
     * @brief Load the codec options (both common and private options).
     * @note See libav flag AV_OPT_FLAG_XXX (default = 0: no flag restriction).
     * @see openCodec to load private codec options.
     */
    void loadCodecOptions(const int flags = 0);

    /**
     * @return codec options as array
     * @see loadCodecOptions
     */
    OptionArray getOptions();
#ifndef SWIG
    OptionMap& getOptionsMap() { return _options; } ///< Get options as map
#endif
    Option& getOption(const std::string& optionName) { return _options.at(optionName); }

#ifndef SWIG
    AVCodecContext& getAVCodecContext() { return *_avCodecContext; }
    const AVCodecContext& getAVCodecContext() const { return *_avCodecContext; }
    AVCodec& getAVCodec() { return *_avCodec; }
    const AVCodec& getAVCodec() const { return *_avCodec; }
#endif

private:
    void setCodec(const ECodecType type, const std::string& codecName);
    void setCodec(const ECodecType type, const AVCodecID codecId);
    void allocateContext();

protected:
    AVCodecContext* _avCodecContext;     ///< Full codec instance description (has ownership)
    AVCodec* _avCodec;                   ///< Codec abstract description
    const bool _isCodecContextAllocated; ///< Is the AVCodecContext allocated by the class

    ECodecType _type;

    OptionMap _options;
};
}

#endif
