#pragma once
#include "../Managers/TextManager.h"
#include <functional>
#include <SDL2/SDL.h>
#include <string>

class Button final
{
public:
	using ClickCallBack = std::function<void()>;

	Button(const SDL_Rect& bounds);

	void SetBounds(const SDL_Rect& bounds);
	void SetHovered(bool isHovered);
	void SetOnClick(ClickCallBack onClick);
	void SetTextName(const std::string& textName);
	void HandleEvent(const SDL_Event& event);
	void Render(SDL_Renderer* renderer, TextManager& textManager) const;
	void Click();

	bool ContainsPoint(int x, int y) const;

	inline bool IsHovered() const { return m_isHovered; }
	inline const SDL_Rect& GetBounds() const { return m_bounds; }


private:
	ClickCallBack m_onClick;

	SDL_Rect m_bounds{ 0, 0, 0, 0 };
	std::string m_textName;

	bool m_isHovered{ false };


private: 
	void OnClick();
};