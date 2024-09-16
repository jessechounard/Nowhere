#include <dr_wav.h>
#include <spdlog/spdlog.h>

#include <Lucky/Audio/Sound.hpp>

namespace Lucky
{
    Sound::Sound(const std::string &filename)
    {
        unsigned int drwavChannels;
        unsigned int drwavSampleRate;
        drwav_uint64 drwavSampleCount;
        drwav_int16 *drwavSamples = drwav_open_file_and_read_pcm_frames_s16(
            filename.c_str(), &drwavChannels, &drwavSampleRate, &drwavSampleCount, NULL);

        if (!drwavSamples)
        {
            spdlog::error("Failed to open sound file {}", filename);
            throw;
        }

        frameCount = static_cast<uint32_t>(drwavSampleCount);
        sampleRate = drwavSampleRate;
        channels = drwavChannels;
        frames.insert(frames.end(), drwavSamples, drwavSamples + frameCount * channels);

        drwav_free(drwavSamples, nullptr);
    }

    Sound::Sound(void *buffer, uint32_t bufferByteSize)
    {
        unsigned int drwavChannels;
        unsigned int drwavSampleRate;
        drwav_uint64 drwavSampleCount;
        drwav_int16 *drwavSamples = drwav_open_memory_and_read_pcm_frames_s16(
            buffer, bufferByteSize, &drwavChannels, &drwavSampleRate, &drwavSampleCount, NULL);

        if (!drwavSamples)
        {
            spdlog::error("Failed to open sound from buffer");
            throw;
        }

        frameCount = static_cast<uint32_t>(drwavSampleCount);
        sampleRate = drwavSampleRate;
        channels = drwavChannels;
        frames.insert(frames.end(), drwavSamples, drwavSamples + frameCount);

        drwav_free(drwavSamples, nullptr);
    }

} // namespace Lucky