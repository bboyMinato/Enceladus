#include "SoundManager.h"
#include <SDL2/SDL.h>

SoundManager::~SoundManager()
{
	StopMusic();
	UnloadAllSounds();
	UnloadAllMusic();

	if (m_initialized)
	{
		Mix_CloseAudio();
		m_initialized = false;
	}

	if (m_mixInitFlags != 0)
	{
		Mix_Quit();
		m_mixInitFlags = 0;
	}
}

bool SoundManager::Init()
{
	if (m_initialized)
	{
		return true;
	}

	const int requestedFlags = MIX_INIT_OGG;
	m_mixInitFlags = Mix_Init(requestedFlags);

	if ((m_mixInitFlags & requestedFlags) != requestedFlags)
	{
		SDL_Log("Failed to initialize SDL_mixer with OGG support: %s", Mix_GetError());

		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
	{
		SDL_Log("Failed to open audio device: %s", Mix_GetError());

		if (m_mixInitFlags != 0)
		{
			Mix_Quit();
			m_mixInitFlags = 0;
		}

		return false;
	}

	Mix_AllocateChannels(32);
	m_initialized = true;
	
	return true;
}

bool SoundManager::LoadSound(const std::string& soundName, const std::string& filePath)
{
	if (!m_initialized)
	{
		SDL_Log("SoundManager not initialized. Call Init() before loading sounds.");
		return false;
	}

	Mix_Chunk* sound = Mix_LoadWAV(filePath.c_str());
	if (!sound)
	{
		SDL_Log("Failed to load sound: %s", Mix_GetError());
		return false;
	}

	auto existingSoundIt = m_sounds.find(soundName);
	if (existingSoundIt != m_sounds.end())
	{
		Mix_FreeChunk(existingSoundIt->second);
	}

	m_sounds[soundName] = sound;
	
	return true;
}

bool SoundManager::LoadMusic(const std::string& musicName, const std::string& filePath)
{
	if (!m_initialized)
	{
		SDL_Log("SoundManager not initialized. Call Init() before loading music.");
		return false;
	}
	
	Mix_Music* music = Mix_LoadMUS(filePath.c_str());
	if (!music)
	{
		SDL_Log("Failed to load music: %s", Mix_GetError());
		return false;
	}
	
	auto existingMusicIt = m_music.find(musicName);
	if (existingMusicIt != m_music.end())
	{
		Mix_FreeMusic(existingMusicIt->second);
	}
	
	m_music[musicName] = music;
	
	return true;
}

void SoundManager::UnloadSound(const std::string& soundName)
{
	auto it = m_sounds.find(soundName);
	if (it != m_sounds.end())
	{
		Mix_FreeChunk(it->second);
		m_sounds.erase(it);
	}
}

void SoundManager::UnloadMusic(const std::string& musicName)
{
	auto it = m_music.find(musicName);
	if (it != m_music.end())
	{
		Mix_FreeMusic(it->second);
		m_music.erase(it);
	}
}

void SoundManager::UnloadAllSounds()
{
	for (auto& pair : m_sounds)
	{
		(void)pair.first;
		Mix_FreeChunk(pair.second);
	}

	m_sounds.clear();
}

void SoundManager::UnloadAllMusic()
{
	for (auto& pair : m_music)
	{
		(void)pair.first;
		Mix_FreeMusic(pair.second);
	}

	m_music.clear();
}

void SoundManager::PlaySound(const std::string& soundName, int loops, int channel) const
{
	Mix_Chunk* sound = GetSound(soundName);
	if (!sound)
	{
		SDL_Log("Sound '%s' not found. Make sure it is loaded before playing.", soundName.c_str());
		return;
	}
	
	if (Mix_PlayChannel(channel, sound, loops) == -1)
	{
		SDL_Log("Failed to play sound '%s': %s", soundName.c_str(), Mix_GetError());
		return;
	}
}

void SoundManager::PlayMusic(const std::string& musicName, int loops) const
{
	if (Mix_PlayingMusic())
	{
		return;
	}

	Mix_Music* music = GetMusic(musicName);
	if (!music)
	{
		SDL_Log("Music '%s' not found. Make sure it is loaded before playing.", musicName.c_str());
		return;
	}
	
	if (Mix_PlayMusic(music, loops) == -1)
	{
		SDL_Log("Failed to play music '%s': %s", musicName.c_str(), Mix_GetError());
		return;
	}
}

void SoundManager::StopMusic(int channel) const
{
	if (Mix_PlayingMusic())
	{
		Mix_HaltMusic();
	}
}

void SoundManager::PauseMusic() const
{
	if (Mix_PlayingMusic())
	{
		Mix_PauseMusic();
	}
}

void SoundManager::ResumeMusic() const
{
	if (Mix_PausedMusic())
	{
		Mix_ResumeMusic();
	}
}

void SoundManager::SetSoundVolume(int volume) const
{
	if (volume < 0)
	{
		volume = 0;
	}
	else if (volume > MIX_MAX_VOLUME)
	{
		volume = MIX_MAX_VOLUME;
	}

	Mix_Volume(-1, volume);
}

void SoundManager::SetMusicVolume(int volume) const
{
	if (volume < 0)
	{
		volume = 0;
	}
	else if (volume > MIX_MAX_VOLUME)
	{
		volume = MIX_MAX_VOLUME;
	}

	Mix_VolumeMusic(volume);
}

Mix_Chunk* SoundManager::GetSound(const std::string& soundName) const
{
	const auto it = m_sounds.find(soundName);
	if (it != m_sounds.end())
	{
		return it->second;
	}

	return nullptr;
}

Mix_Music* SoundManager::GetMusic(const std::string& musicName) const
{
	const auto it = m_music.find(musicName);
	if (it != m_music.end())
	{
		return it->second;
	}

	return nullptr;
}