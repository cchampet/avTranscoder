#include "Sound.hpp"

#include "SDL/SDL.h"

#include <AvTranscoder/properties/print.hpp>


avtranscoder::AudioReader* Sound::_reader = NULL;
unsigned char* Sound::_audioChunk = 0;
size_t Sound::_audioLength = 0;
unsigned char* Sound::_audioPosition = 0;

Sound::Sound(avtranscoder::AudioReader& reader)
{
  _reader = &reader;

  // open the audio device
  SDL_AudioSpec wanted;

    /* Set the audio format */
    wanted.freq = 22050;
    wanted.format = AUDIO_S16;
    wanted.channels = 2;    /* 1 = mono, 2 = stereo */
    wanted.samples = 1024;  /* Good low-latency value for callback */
    wanted.callback = fill_audio;
    wanted.userdata = NULL;

    /* Open the audio device, forcing the desired format */
    if ( SDL_OpenAudio(&wanted, NULL) < 0 ) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        return(-1);
    }
}

void Sound::fillAudio(void* data, unsigned char* stream, size_t len)
{
  /* Only play if we have data left */
        if ( _audioLength == 0 )
            return;

        /* Mix as much data as possible */
        const size_t len = ( len > _audioLength ? _audioLength : len );
        SDL_MixAudio(stream, _audioPosition, len, SDL_MIX_MAXVOLUME);
        _audioPosition += len;
        _audioLength -= len;
}
