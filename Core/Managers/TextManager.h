#pragma once
#include <string>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class TextManager
{
public:

	~TextManager();

	bool Init(SDL_Renderer* renderer);
	void Shutdown();

	bool LoadFont(const std::string& fontName, const std::string& filePath, int fontSize);
	void UnloadFont(const std::string& fontName);
	void UnloadAllFonts();

	bool LoadText(const std::string& textName, const std::string& fontName, const std::string& text, SDL_Color color);
	void UnloadText(const std::string& textName);
	void UnloadAllTexts();

	SDL_Texture* GetText(const std::string& textName) const;
	void GetTextSize(const std::string& textName, int& width, int& height) const;

private:
	TTF_Font* GetFont(const std::string& fontName) const;

private:
	SDL_Renderer* m_renderer{ nullptr };
	bool m_isInitialized{ false };

	std::unordered_map<std::string, TTF_Font*> m_fonts;
	std::unordered_map<std::string, SDL_Texture*> m_texts;
};