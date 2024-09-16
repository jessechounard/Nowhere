#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace Lucky
{
    struct Sound
    {
        uint32_t frameCount;
        uint16_t sampleRate;
        uint16_t channels;
        std::vector<int16_t> frames;

        Sound(const std::string &filename);
        Sound(void *buffer, uint32_t bufferByteSize);
        ~Sound() = default;
    };
} // namespace Lucky
