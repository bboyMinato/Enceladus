#pragma once
#include <SDL2/SDL_mixer.h>
#include <string> 
#include <unordered_map>

class SoundManager
{
public:
	~SoundManager();

	bool Init(); 
	bool LoadSound(const std::string& soundName, const std::string& filePath);
	bool LoadMusic(const std::string& musicName, const std::string& filePath);

	void UnloadSound(const std::string& soundName);
	void UnloadMusic(const std::string& musicName);
	void UnloadAllSounds();
	void UnloadAllMusic();

	void PlaySound(const std::string& soundName, int loops = 0, int channel = -1) const;
	void PlayMusic(const std::string& musicName, int loops = -1) const;

	void StopMusic(int channel = -1) const;
	void PauseMusic() const;
	void ResumeMusic() const;

	void SetSoundVolume(int volume) const;
	void SetMusicVolume(int volume) const;

private:
	Mix_Chunk* GetSound(const std::string& soundName) const;
	Mix_Music* GetMusic(const std::string& musicName) const;

	bool m_initialized{ false };
	int m_mixInitFlags{ 0 };

	std::unordered_map<std::string, Mix_Chunk*> m_sounds;
	std::unordered_map<std::string, Mix_Music*> m_music;
};