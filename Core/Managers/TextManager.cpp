#include "TextManager.h"

TextManager::~TextManager()
{
	Shutdown();
}

bool TextManager::Init(SDL_Renderer* renderer)
{
	if (m_isInitialized)
	{
		return true;
	}

	if (!renderer)
	{
		SDL_Log("Failed to initialize SDL_ttf: %s", TTF_GetError());
	
		return false;
	}

	if (TTF_Init() == -1)
	{
		SDL_Log("Failed to initialize SDL_ttf: %s", TTF_GetError());
	
		return false;
	}

	m_renderer = renderer;
	m_isInitialized = true;

	return true;
}

void TextManager::Shutdown()
{
	UnloadAllFonts();
	UnloadAllTexts();

	if (m_isInitialized)
	{
		TTF_Quit();
		m_isInitialized = false;
	}

	m_renderer = nullptr;
}

bool TextManager::LoadFont(const std::string& fontName, const std::string& filePath, int fontSize)
{
	if (!m_isInitialized)
	{
		SDL_Log("TextManager is not initialized.");
		return false;
	}

	if (fontSize <= 0)
	{
		SDL_Log("Invalid font size: %d", fontSize);
		return false;
	}

	TTF_Font* font = TTF_OpenFont(filePath.c_str(), fontSize);
	if (!font)
	{
		SDL_Log("Failed to load font: %s", TTF_GetError());
		return false;
	}

	auto existingFontIt = m_fonts.find(fontName);
	if (existingFontIt != m_fonts.end())
	{
		TTF_CloseFont(existingFontIt->second);
	}

	m_fonts[fontName] = font;

	return true;
}

void TextManager::UnloadFont(const std::string& fontName)
{
	const auto it = m_fonts.find(fontName);
	if (it != m_fonts.end())
	{
		TTF_CloseFont(it->second);
		m_fonts.erase(it);
	}
}

void TextManager::UnloadAllFonts()
{
	for (auto& pair : m_fonts)
	{
		(void)pair.first;
		TTF_CloseFont(pair.second);
	}

	m_fonts.clear();
}

bool TextManager::LoadText(const std::string& textName, const std::string& fontName, const std::string& text, SDL_Color color)
{
	if (!m_isInitialized)
	{
		SDL_Log("TextManager is not initialized.");
		return false;
	}

	if (!m_renderer)
	{
		SDL_Log("Renderer is not set.");
		return false;
	}

	TTF_Font* font = GetFont(fontName);
	if (!font)
	{
		SDL_Log("Font '%s' not found.", fontName.c_str());
		return false;
	}

	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
	if (!surface)
	{
		SDL_Log("Failed to render text surface: %s", TTF_GetError());
		return false;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);
	
	if (!texture)
	{
		SDL_Log("Failed to create text texture: %s", SDL_GetError());
		return false;
	}

	auto it = m_texts.find(textName);
	if (it != m_texts.end())
	{
		SDL_DestroyTexture(it->second);
	}

	m_texts[textName] = texture;

	return true;
}

void TextManager::UnloadText(const std::string& textName)
{
	const auto it = m_texts.find(textName);
	if (it != m_texts.end())
	{
		SDL_DestroyTexture(it->second);
		m_texts.erase(it);
	}
}

void TextManager::UnloadAllTexts()
{
	for (auto& pair : m_texts)
	{
		(void)pair.first;
		SDL_DestroyTexture(pair.second);
	}

	m_texts.clear();
}

SDL_Texture* TextManager::GetText(const std::string& textName) const
{
	const auto it = m_texts.find(textName);
	if (it != m_texts.end())
	{
		return it->second;
	}

	return nullptr;
}

void TextManager::GetTextSize(const std::string& textName, int& width, int& height) const
{
	width = 0;
	height = 0; 

	SDL_Texture* texture = GetText(textName);
	
	if (!texture)
	{
		return;
	}

	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
}

TTF_Font* TextManager::GetFont(const std::string& fontName) const
{
	const auto it = m_fonts.find(fontName);
	if (it != m_fonts.end())
	{
		return it->second;
	}

	return nullptr;
}