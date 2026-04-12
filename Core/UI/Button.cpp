#include "Button.h"

Button::Button(const SDL_Rect& bounds)
	: m_bounds(bounds)
{
}

void Button::SetBounds(const SDL_Rect& bounds)
{
	m_bounds = bounds;
}

void Button::SetHovered(bool isHovered)
{
	m_isHovered = isHovered;
}

void Button::SetOnClick(ClickCallBack onClick)
{
	m_onClick = onClick;
}

void Button::SetTextName(const std::string& textName)
{
	m_textName = textName;
}

void Button::HandleEvent(const SDL_Event& event)
{
	if (event.type == SDL_MOUSEMOTION)
	{
		m_isHovered = ContainsPoint(event.motion.x, event.motion.y);
		return;
	}

	if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
	{
		if (m_isHovered && ContainsPoint(event.button.x, event.button.y))
		{
			OnClick();

			return;
		}		
	}
}

void Button::Render(SDL_Renderer* renderer, TextManager& textManager) const
{
	if (renderer == nullptr)
	{
		return;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	const SDL_Rect shadowRect
	{
		m_bounds.x + 6,
		m_bounds.y + 6,
		m_bounds.w,
		m_bounds.h
	};

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 110);
	SDL_RenderFillRect(renderer, &shadowRect);

	const SDL_Color baseFill = m_isHovered
		? SDL_Color{ 92, 68, 32, 210 }
	: SDL_Color{ 52, 38, 18, 190 };

	const SDL_Color borderColor = m_isHovered
		? SDL_Color{ 212, 175, 55, 255 }
	: SDL_Color{ 120, 90, 40, 220 };

	SDL_SetRenderDrawColor(renderer, baseFill.r, baseFill.g, baseFill.b, baseFill.a);
	SDL_RenderFillRect(renderer, &m_bounds);

	SDL_Rect topHighlight
	{
		m_bounds.x + 2,
		m_bounds.y + 2,
		m_bounds.w - 4,
		6
	};

	SDL_SetRenderDrawColor(renderer, 255, 240, 200, m_isHovered ? 70 : 35);
	SDL_RenderFillRect(renderer, &topHighlight);

	SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
	SDL_RenderDrawRect(renderer, &m_bounds);

	if (m_isHovered)
	{
		SDL_Rect innerBorder
		{
			m_bounds.x + 1,
			m_bounds.y + 1,
			m_bounds.w - 2,
			m_bounds.h - 2
		};

		SDL_SetRenderDrawColor(renderer, 255, 220, 120, 140);
		SDL_RenderDrawRect(renderer, &innerBorder);
	}

	if (m_textName.empty())
	{
		return;
	}

	SDL_Texture* textTexture = textManager.GetText(m_textName);
	if (textTexture == nullptr)
	{
		return;
	}

	int textWidth = 0;
	int textHeight = 0;
	textManager.GetTextSize(m_textName, textWidth, textHeight);

	SDL_Rect textRect
	{
		m_bounds.x + (m_bounds.w - textWidth) / 2,
		m_bounds.y + (m_bounds.h - textHeight) / 2,
		textWidth,
		textHeight
	};

	SDL_Rect textShadowRect
	{
		textRect.x + 3,
		textRect.y + 3,
		textRect.w,
		textRect.h
	};

	SDL_SetTextureColorMod(textTexture, 0, 0, 0);
	SDL_SetTextureAlphaMod(textTexture, 170);
	SDL_RenderCopy(renderer, textTexture, nullptr, &textShadowRect);

	if (m_isHovered)
	{
		SDL_SetTextureColorMod(textTexture, 255, 230, 160);
	}
	else
	{
		SDL_SetTextureColorMod(textTexture, 235, 225, 210);
	}

	SDL_SetTextureAlphaMod(textTexture, 255);
	SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

	SDL_SetTextureColorMod(textTexture, 255, 255, 255);
	SDL_SetTextureAlphaMod(textTexture, 255);
}

void Button::Click()
{
	OnClick();
}

bool Button::ContainsPoint(int x, int y) const
{
	const SDL_Point point{ x, y };
	return SDL_PointInRect(&point, &m_bounds);
}

void Button::OnClick()
{
	if (m_onClick)
	{
		m_onClick();
	}
}