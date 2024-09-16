#include <map>

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include <Lucky/Audio/AudioPlayer.hpp>
#include <Lucky/Audio/Sound.hpp>
#include <Lucky/Math/MathHelpers.hpp>

namespace Lucky
{
    enum class AudioType
    {
        Sound,
        Stream
    };

    enum class AudioState
    {
        Playing,
        Paused
    };

    struct AudioInstance
    {
        AudioInstance()
            : audioStream(nullptr),
              audioPlayer(nullptr),
              loop(false),
              audioCallback(nullptr),
              audioState(AudioState::Playing)
        {
        }

        virtual ~AudioInstance()
        {
        }

        virtual uint32_t PutSamplesSDL(int bytesRequested) = 0;

        SDL_AudioStream *audioStream;
        AudioPlayer *audioPlayer;
        bool loop;
        AudioCallbackFunction audioCallback;
        AudioState audioState;
        std::string soundGroupName;
    };

    struct SoundInstance : public AudioInstance
    {
        SoundInstance()
            : samplePosition(0)
        {
        }

        uint32_t PutSamplesSDL(int bytesRequested)
        {
            uint32_t framesRequested = bytesRequested / sizeof(int16_t) / sound->channels;
            uint32_t framesAvailable = sound->frameCount - samplePosition;
            uint32_t framesToPut = std::min(framesRequested, framesAvailable);

            if (framesToPut > 0)
            {
                int16_t *position = &sound->frames[0] + samplePosition * sound->channels;
                int bytesToPut = framesToPut * sizeof(int16_t) * sound->channels;

                float groupVolume = audioPlayer->GetGroupVolume(soundGroupName);
                float defaultVolume = audioPlayer->GetGroupVolume("default");
                float volume = std::min(groupVolume, defaultVolume);

                if (ApproximatelyEqual(volume, 1.0f))
                {
                    SDL_PutAudioStreamData(audioStream, position, bytesToPut);
                }
                else
                {
                    std::vector<int16_t> workBuffer(position, position + framesToPut * sound->channels);

                    std::transform(workBuffer.begin(), workBuffer.end(), workBuffer.begin(),
                        [volume](int16_t sample) -> int16_t { return (int16_t)(sample * volume); });

                    SDL_PutAudioStreamData(audioStream, &workBuffer[0], bytesToPut);
                }

                samplePosition += framesToPut;
                if (loop && samplePosition == sound->frameCount)
                {
                    samplePosition = 0;
                    return bytesToPut + PutSamplesSDL(bytesRequested - bytesToPut);
                }
                else
                {
                    return bytesToPut;
                }
            }
            return 0;
        }

        std::shared_ptr<Sound> sound;
        uint32_t samplePosition;
    };

    struct StreamInstance : public AudioInstance
    {
        uint32_t PutSamplesSDL(int bytesRequested)
        {
        }
    };

    struct SoundGroup
    {
        SDL_AudioDeviceID audioDeviceId = 0;
        SDL_AudioDeviceID logicalAudioDeviceId = 0;
        float volume = 0;
    };

    struct AudioPlayer::Impl
    {
        AudioRef nextAudioRef;
        SoundGroupSettings defaultSoundGroupSettings;

        std::map<AudioRef, AudioInstance *> audioInstances;
        std::map<std::string, SoundGroup> soundGroups;
    };

    AudioPlayer::AudioPlayer(SoundGroupSettings defaultSoundGroupSettings)
        : pImpl(std::make_unique<Impl>())
    {
        pImpl->nextAudioRef = 1;
        pImpl->defaultSoundGroupSettings = defaultSoundGroupSettings;

        CreateSoundGroup("default", defaultSoundGroupSettings);
    }

    AudioPlayer::~AudioPlayer()
    {
        for (auto const &i : pImpl->audioInstances)
        {
            SDL_DestroyAudioStream(i.second->audioStream);
            delete i.second;
        }

        pImpl->audioInstances.clear();

        for (auto const &i : pImpl->soundGroups)
        {
            SDL_CloseAudioDevice(i.second.logicalAudioDeviceId);
        }
    }

    void AudioPlayer::CreateSoundGroup(const std::string &soundGroupName, SoundGroupSettings settings)
    {
        auto groupIterator = pImpl->soundGroups.find(soundGroupName);
        if (groupIterator != pImpl->soundGroups.end())
        {
            SoundGroup &soundGroup = groupIterator->second;

            // if the device id is changing
            //    create the new one
            //    loop through all playing instances and rebind to the new one
            //    destroy the old one
            if (soundGroup.audioDeviceId != settings.deviceId)
            {
                SDL_AudioDeviceID newDeviceId = SDL_OpenAudioDevice(settings.deviceId, nullptr);

                for (auto const &i : pImpl->audioInstances)
                {
                    if (i.second->soundGroupName == soundGroupName && i.second->audioState == AudioState::Playing)
                    {
                        SDL_UnbindAudioStream(i.second->audioStream);
                        SDL_BindAudioStream(newDeviceId, i.second->audioStream);
                    }
                }

                SDL_CloseAudioDevice(soundGroup.logicalAudioDeviceId);

                soundGroup.audioDeviceId = settings.deviceId;
                soundGroup.logicalAudioDeviceId = newDeviceId;
            }

            soundGroup.volume = settings.volume;
        }
        else
        {
            SoundGroup soundGroup;

            soundGroup.audioDeviceId = settings.deviceId;
            soundGroup.logicalAudioDeviceId = SDL_OpenAudioDevice(settings.deviceId, nullptr);
            if (soundGroup.logicalAudioDeviceId == 0)
            {
                spdlog::error("Failed to open audio device with DeviceID {}", settings.deviceId);
                throw;
            }
            soundGroup.volume = settings.volume;
            pImpl->soundGroups[soundGroupName] = soundGroup;
        }
    }

    void AudioPlayer::DestroySoundGroup(const std::string &soundGroupName)
    {
        StopGroup(soundGroupName);

        auto groupIterator = pImpl->soundGroups.find(soundGroupName);
        if (groupIterator != pImpl->soundGroups.end())
        {
            SDL_CloseAudioDevice(groupIterator->second.logicalAudioDeviceId);
            pImpl->soundGroups.erase(groupIterator);
        }
    }

    void AudioPlayer::StopGroup(const std::string &soundGroupName)
    {
        // using this for loop configuration instead of a range-based for loop
        // because we might be erasing elements while iterating through it
        for (auto current = pImpl->audioInstances.cbegin(), next = current; current != pImpl->audioInstances.cend();
             current = next)
        {
            ++next;
            AudioInstance *ai = current->second;
            if (ai->soundGroupName == soundGroupName)
            {
                SDL_DestroyAudioStream(ai->audioStream);
                delete ai;
                pImpl->audioInstances.erase(current);
            }
        }
    }

    void AudioPlayer::PauseGroup(const std::string &soundGroupName)
    {
        for (auto const &i : pImpl->audioInstances)
        {
            if (i.second->soundGroupName == soundGroupName)
            {
                Pause(i.first);
            }
        }
    }

    void AudioPlayer::ResumeGroup(const std::string &soundGroupName)
    {
        for (auto const &i : pImpl->audioInstances)
        {
            if (i.second->soundGroupName == soundGroupName)
            {
                Resume(i.first);
            }
        }
    }

    SDL_AudioDeviceID AudioPlayer::GetGroupDeviceId(const std::string &soundGroupName)
    {
        auto iterator = pImpl->soundGroups.find(soundGroupName);
        if (iterator == pImpl->soundGroups.end())
        {
            spdlog::error("Couldn't find audio group named {}", soundGroupName);
            throw;
        }

        return iterator->second.logicalAudioDeviceId;
    }

    float AudioPlayer::GetGroupVolume(const std::string &soundGroupName)
    {
        auto iterator = pImpl->soundGroups.find(soundGroupName);
        if (iterator == pImpl->soundGroups.end())
        {
            spdlog::error("Couldn't find audio group named {}", soundGroupName);
            throw;
        }

        return iterator->second.volume;
    }

    AudioRef AudioPlayer::Play(std::shared_ptr<Sound> sound, const std::string &soundGroupName, const bool beginPaused,
        const bool loop, AudioCallbackFunction audioCallback)
    {
        auto groupIterator = pImpl->soundGroups.find(soundGroupName);
        if (groupIterator == pImpl->soundGroups.end())
        {
            CreateSoundGroup(soundGroupName, pImpl->defaultSoundGroupSettings);
        }

        SoundGroup &soundGroup = pImpl->soundGroups[soundGroupName];

        SoundInstance *si = new SoundInstance();
        si->sound = sound;
        si->audioPlayer = this;
        si->loop = loop;
        si->audioCallback = audioCallback;
        si->audioState = beginPaused ? AudioState::Paused : AudioState::Playing;
        si->soundGroupName = soundGroupName;

        SDL_AudioSpec sourceSpec;
        sourceSpec.channels = sound->channels;
        sourceSpec.freq = sound->sampleRate;
        sourceSpec.format = SDL_AUDIO_S16;

        SDL_AudioSpec outputSpec;
        outputSpec.channels = 2;
        outputSpec.freq = 48000;
        outputSpec.format = SDL_AUDIO_S16;

        SDL_AudioStream *audioStream = SDL_CreateAudioStream(&sourceSpec, &outputSpec);
        if (!audioStream)
        {
            spdlog::error("Failed to create audio stream");
        }

        si->audioStream = audioStream;

        SDL_SetAudioStreamGetCallback(audioStream, &SDLCallback, si);
        if (!beginPaused)
        {
            SDL_BindAudioStream(soundGroup.logicalAudioDeviceId, audioStream);
        }

        pImpl->audioInstances[pImpl->nextAudioRef] = si;
        return pImpl->nextAudioRef++;
    }

    void AudioPlayer::Pause(const AudioRef &audioRef)
    {
        auto iterator = pImpl->audioInstances.find(audioRef);
        if (iterator == pImpl->audioInstances.end() || iterator->second->audioState == AudioState::Paused)
        {
            return;
        }

        AudioInstance *ai = iterator->second;
        ai->audioState = AudioState::Paused;
        SDL_UnbindAudioStream(ai->audioStream);
    }

    void AudioPlayer::Resume(const AudioRef &audioRef)
    {
        auto iterator = pImpl->audioInstances.find(audioRef);
        if (iterator == pImpl->audioInstances.end() || iterator->second->audioState == AudioState::Playing)
        {
            return;
        }

        AudioInstance *ai = iterator->second;
        ai->audioState = AudioState::Playing;
        SDL_BindAudioStream(ai->audioPlayer->GetGroupDeviceId(ai->soundGroupName), ai->audioStream);
    }

    void AudioPlayer::Stop(const AudioRef &audioRef)
    {
        auto iterator = pImpl->audioInstances.find(audioRef);
        if (iterator == pImpl->audioInstances.end())
        {
            return;
        }

        AudioInstance *ai = iterator->second;
        SDL_DestroyAudioStream(ai->audioStream);
        delete ai;
        pImpl->audioInstances.erase(audioRef);
    }

    void SDLCallback(void *userData, SDL_AudioStream *stream, int additionalAmount, int totalBytes)
    {
        AudioInstance *ai = (AudioInstance *)userData;
        ai->audioPlayer->AudioCallback(ai, totalBytes);
    }

    void AudioPlayer::AudioCallback(AudioInstance *si, int bytesRequested)
    {
        si->PutSamplesSDL(bytesRequested);
    }
} // namespace Lucky
