#include "AppSetting.h"
#include <algorithm>
#include <fstream>

namespace
{
	std::string Trim(const std::string& str)
	{
		const std::size_t start = str.find_first_not_of(" \t\n\r");
		if (start == std::string::npos)
		{
			return {};
		}

		const std::size_t end = str.find_last_not_of(" \t\n\r");
		
		return str.substr(start, end - start + 1);
	}

	int ClampVolume(int volume)
	{
		return std::clamp(volume, 0, 128);
	}
}

AppSettings AppSettings::LoadFromFile(const std::string& filePath)
{
	AppSettings settings{};

	std::ifstream file(filePath);
	if (!file.is_open())
	{
		return settings;
	}

	std::string currentSection;
	std::string line; 

	while (std::getline(file, line))
	{
		line = Trim(line);

		if (line.empty() || line.starts_with(';') || line.starts_with('#'))
		{
			continue;
		}

		if (line.front() == '[' && line.back() == ']')
		{
			currentSection = Trim(line.substr(1, line.size() - 2));
			continue;
		}

		const std::size_t equalsIndex = line.find('=');
		if (equalsIndex == std::string::npos)
		{
			continue;
		}

		const std::string key = Trim(line.substr(0, equalsIndex));
		const std::string value = Trim(line.substr(equalsIndex + 1));

		try
		{
			const int parsedValue = std::stoi(value);

			if (currentSection == "Window")
			{
				if (key == "Width")
				{
					settings.windowSettings.width = parsedValue;
				}
				else if (key == "Height")
				{
					settings.windowSettings.height = parsedValue;
				}
				else if (key == "PosX")
				{
					settings.windowSettings.posX = parsedValue;
				}
				else if (key == "PosY")
				{
					settings.windowSettings.posY = parsedValue;
				}
				else if (key == "Flags")
				{
					settings.windowSettings.flags = parsedValue;
				}
			}
			else if (currentSection == "Audio")
			{
				if (key == "MusicVolume")
				{
					settings.audioSettings.musicVolume = ClampVolume(parsedValue);
				}
				else if (key == "SoundVolume")
				{
					settings.audioSettings.soundVolume = ClampVolume(parsedValue);
				}
			}
		}
		catch (const std::exception&)
		{
		}
	}

	return settings;
}

bool AppSettings::SaveToFile(const std::string& filePath) const
{
	std::ofstream file(filePath);
	if (!file.is_open())
	{
		return false;
	}

	file << "[Window]\n";
	file << "Width=" << windowSettings.width << '\n';
	file << "Height=" << windowSettings.height << '\n';
	file << "PosX=" << windowSettings.posX << '\n';
	file << "PosY=" << windowSettings.posY << '\n';
	file << "Flags=" << windowSettings.flags << '\n';
	file << '\n';
	file << "[Audio]\n";
	file << "MusicVolume=" << ClampVolume(audioSettings.musicVolume) << '\n';
	file << "SoundVolume=" << ClampVolume(audioSettings.soundVolume) << '\n';


	return true;
}