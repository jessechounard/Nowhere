#include <stdint.h>

namespace Lucky
{
    // a simple sound effect, loaded fully into memory
    class Sound
    {
        uint32_t sampleCount;
        uint16_t sampleRate;
        uint16_t channels;
        int16_t samples[1];
    };

    // a compressed sound, decoded and streamed at runtime
    class Song
    {
    };

    class AudioPlayer
    {
      public:
        // todo: Do we need to allow select something other than the default output?
        AudioPlayer();
        AudioPlayer(const AudioPlayer &) = delete;
        AudioPlayer(const AudioPlayer &&) = delete;
        ~AudioPlayer();

        AudioPlayer &operator=(const AudioPlayer &) = delete;
        AudioPlayer &operator=(const AudioPlayer &&) = delete;

        /*
        * create audio group
        * stop audio group (with fade or immediate)
        * pause audio group (with fade or immediate)
        * resume audio group (fade back in?)
        * 
        * play song (on group or create group)
        * play sound (on group or create group)
        * 
        * pause all (with fade or immediate)
        * stop all (with fade or immediate)
        * 
        */

      private:
    };
}