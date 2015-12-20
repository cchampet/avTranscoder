#ifndef _AVPLAY_SOUND_HPP_
#define _AVPLAY_SOUND_HPP_

#include <AvTranscoder/reader/AudioReader.hpp>

class Sound
{
public:
    Sound(avtranscoder::AudioReader& reader);

    void launch();

private:
    static void display();

    static void fillAudio(void* data, unsigned char* stream, size_t len);

private:
    static avtranscoder::AudioReader* _reader;

    static unsigned char* _audioChunk;
    static size_t _audioLength;
    static unsigned char* _audioPosition;
};

#endif
