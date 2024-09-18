#pragma once

#include <string>

#include <dr_mp3.h>
#include <stb_vorbis.h>

namespace Lucky
{
    class Stream
    {
      public:
        Stream(const std::string &fileName);
        Stream(void *buffer, uint32_t bufferByteSize);
        Stream(const Stream &stream);
        ~Stream();

        uint32_t GetFrames(int16_t *buffer, uint32_t frames, bool *loop);

        uint16_t sampleRate;
        uint16_t channels;

      private:
        enum class StreamSource
        {
            File,
            Memory,
        };

        StreamSource streamSource;
        std::string fileName;
        void *buffer;
        uint32_t bufferByteSize;
        stb_vorbis *vorbis;
        drmp3 *mp3;
    };
} // namespace Lucky
