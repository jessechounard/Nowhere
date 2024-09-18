#include <Lucky/Audio/Stream.hpp>
#include <spdlog/spdlog.h>

namespace Lucky
{
    Stream::Stream(const std::string &fileName)
        : sampleRate(0),
          channels(0),
          streamSource(StreamSource::File),
          fileName(fileName),
          buffer(nullptr),
          bufferByteSize(0),
          vorbis(nullptr),
          mp3(nullptr)
    {
        vorbis = stb_vorbis_open_filename(fileName.c_str(), nullptr, nullptr);
        if (vorbis)
        {
            stb_vorbis_info info = stb_vorbis_get_info(vorbis);
            sampleRate = info.sample_rate;
            channels = info.channels;
        }
        else
        {
            mp3 = new drmp3();
            if (drmp3_init_file(mp3, fileName.c_str(), nullptr))
            {
                sampleRate = mp3->sampleRate;
                channels = mp3->channels;
            }
            else
            {
                delete mp3;
                spdlog::error("Couldn't open audio stream from file {}", fileName);
                throw;
            }
        }
    }

    Stream::Stream(void *buffer, uint32_t bufferByteSize)
        : sampleRate(0),
          channels(0),
          streamSource(StreamSource::Memory),
          buffer(buffer),
          bufferByteSize(bufferByteSize),
          vorbis(nullptr),
          mp3(nullptr)
    {
        vorbis = stb_vorbis_open_memory((const unsigned char *)buffer, bufferByteSize, nullptr, nullptr);
        if (vorbis)
        {
            stb_vorbis_info info = stb_vorbis_get_info(vorbis);
            sampleRate = info.sample_rate;
            channels = info.channels;
        }
        else
        {
            mp3 = new drmp3();
            if (drmp3_init_memory(mp3, buffer, bufferByteSize, nullptr))
            {
                sampleRate = mp3->sampleRate;
                channels = mp3->channels;
            }
            else
            {
                delete mp3;
                spdlog::error("Couldn't open audio stream from memory");
                throw;
            }
        }
    }

    Stream::Stream(const Stream &stream)
        : sampleRate(stream.sampleRate),
          channels(stream.channels),
          streamSource(stream.streamSource),
          fileName(stream.fileName),
          buffer(stream.buffer),
          bufferByteSize(stream.bufferByteSize),
          vorbis(nullptr),
          mp3(nullptr)
    {
        if (stream.vorbis)
        {
            if (streamSource == StreamSource::File)
            {
                vorbis = stb_vorbis_open_filename(fileName.c_str(), nullptr, nullptr);
                if (!vorbis)
                {
                    spdlog::error("Couldn't open audio stream from file {}", fileName);
                    throw;
                }
            }
            else
            {
                vorbis = stb_vorbis_open_memory((const unsigned char *)buffer, bufferByteSize, nullptr, nullptr);
                if (!vorbis)
                {
                    spdlog::error("Couldn't open audio stream from memory");
                    throw;
                }
            }
            stb_vorbis_info info = stb_vorbis_get_info(vorbis);
            if (channels != info.channels || sampleRate != info.sample_rate)
            {
                spdlog::error("Unexpected settings on cloned vorbis stream");
                throw;
            }
        }
        else
        {
            mp3 = new drmp3();
            if (streamSource == StreamSource::File)
            {
                if (!drmp3_init_file(mp3, fileName.c_str(), nullptr))
                {
                    delete mp3;
                    spdlog::error("Couldn't open audio stream from file {}", fileName);
                    throw;
                }
            }
            else
            {
                if (!drmp3_init_memory(mp3, buffer, bufferByteSize, nullptr))
                {
                    delete mp3;
                    spdlog::error("Couldn't open audio stream from memory");
                    throw;
                }
            }
            if (channels != mp3->channels || sampleRate != mp3->sampleRate)
            {
                delete mp3;
                spdlog::error("Unexpected settings on cloned mp3 stream");
                throw;
            }
        }
    }

    Stream::~Stream()
    {
        if (vorbis)
        {
            stb_vorbis_close(vorbis);
            vorbis = nullptr;
        }
        if (mp3)
        {
            drmp3_uninit(mp3);
            delete mp3;
            mp3 = nullptr;
        }
    }

    uint32_t Stream::GetFrames(int16_t *buffer, uint32_t frames, bool *loop)
    {
        if (loop)
        {
            *loop = false;
        }

        if (vorbis)
        {
            auto samples = stb_vorbis_get_samples_short_interleaved(vorbis, channels, buffer, frames * channels);
            if (loop && samples < frames)
            {
                *loop = true;
                stb_vorbis_seek_start(vorbis);
                return samples + stb_vorbis_get_samples_short_interleaved(
                                     vorbis, channels, buffer + samples * channels, (frames - samples) * channels);
            }
            else
            {
                return samples;
            }
        }
        else
        {
            auto samples = drmp3_read_pcm_frames_s16(mp3, frames, buffer);
            if (loop && samples < frames)
            {
                *loop = true;
                drmp3_seek_to_pcm_frame(mp3, 0);

                return samples + drmp3_read_pcm_frames_s16(mp3, channels, buffer + samples * channels);
            }
            else
            {
                return samples;
            }
        }

        return 0;
    }
} // namespace Lucky
