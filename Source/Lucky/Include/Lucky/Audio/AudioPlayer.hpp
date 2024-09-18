#pragma once

#include <functional>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include <SDL3/SDL_audio.h>
#include <dr_wav.h>

#include <Lucky/Audio/Sound.hpp>
#include <Lucky/Audio/Stream.hpp>

namespace Lucky
{
    typedef uint32_t AudioRef;

    enum class AudioEvent
    {
        Complete,
        Looped,
        Error
    };

    typedef std::function<void(const AudioRef &, const AudioEvent &)> AudioCallbackFunction;

    struct AudioInstance;

    struct SoundGroupSettings
    {
        SDL_AudioDeviceID deviceId = SDL_AUDIO_DEVICE_DEFAULT_OUTPUT;
        float volume = 1.0f;
    };

    class AudioPlayer
    {
      public:
        AudioPlayer(SoundGroupSettings defaultSoundGroupSettings = SoundGroupSettings());
        ~AudioPlayer();

        void CreateSoundGroup(const std::string &soundGroupName, SoundGroupSettings settings = SoundGroupSettings());
        void DestroySoundGroup(const std::string &soundGroupName);

        void StopGroup(const std::string &soundGroupName);
        void PauseGroup(const std::string &soundGroupName);
        void ResumeGroup(const std::string &soundGroupName);

        SDL_AudioDeviceID GetGroupDeviceId(const std::string &soundGroupName);
        float GetGroupVolume(const std::string &soundGroupName);

        // todo: replace these booleans with enum types
        AudioRef Play(std::shared_ptr<Sound> sound, const std::string &soundGroupName = "default",
            const bool beginPaused = false, const bool loop = false, AudioCallbackFunction audioCallback = nullptr);
        AudioRef Play(std::shared_ptr<Stream> stream, const std::string &soundGroupName = "default",
            const bool beginPaused = false, const bool loop = false, AudioCallbackFunction audioCallback = nullptr);
        void Pause(const AudioRef &audioRef);
        void Resume(const AudioRef &audioRef);
        void Stop(const AudioRef &audioRef);

      private:
        AudioPlayer(const AudioPlayer &) = delete;
        AudioPlayer(const AudioPlayer &&) = delete;
        AudioPlayer &operator=(const AudioPlayer &) = delete;
        AudioPlayer &operator=(const AudioPlayer &&) = delete;

        void AudioCallback(AudioInstance *instance, int bytesRequested);

        friend void SDLCallback(void *userData, SDL_AudioStream *stream, int additionalAmount, int totalBytes);

        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
} // namespace Lucky
