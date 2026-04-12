#include "Menu.h"
#include "../Managers/TextManager.h"

MenuItem::MenuItem(const SDL_Rect& bounds, Action action)
	: m_button(bounds), m_action(std::move(action))
{
}

void MenuItem::SetAction(Action action)
{
	m_action = std::move(action);
}

void MenuItem::SetTextName(const std::string& textName)
{
	m_button.SetTextName(textName);
}

void MenuItem::HandleEvent(const SDL_Event& event)
{
	m_button.HandleEvent(event);
}

void MenuItem::Render(SDL_Renderer* renderer, TextManager& textManager) const
{
	m_button.Render(renderer, textManager);
}

void MenuItem::SetHovered(bool isHovered)
{
	m_button.SetHovered(isHovered);
}

void MenuItem::Activate()
{
	if (m_action)
	{
		m_action();
	}
}

bool MenuItem::IsHovered() const
{
	return m_button.IsHovered();
}

bool MenuItem::ContainsPoint(int x, int y) const
{
	return m_button.ContainsPoint(x, y);
}

void Menu::AddMenuItem(const SDL_Rect& bounds, const std::string& textName, MenuItem::Action action)
{
	m_menuItems.emplace_back(bounds, std::move(action));
	m_menuItems.back().SetTextName(textName);

	if (m_menuItems.size() == 1)
	{
		m_selectedIndex = 0;
		UpdateSelection();
	}
}

void Menu::Clear()
{
	m_menuItems.clear();
	m_selectedIndex = 0;
}

void Menu::HandleEvent(const SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_DOWN:
			SelectNextItem();
			return;

		case SDLK_UP:
			SelectPreviousItem();
			return;

		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			ActivateSelectedItem();
			return;

		default:
			break;
		}
	}

	if (event.type == SDL_MOUSEMOTION)
	{
		SyncSelectionFromMouse(event.motion.x, event.motion.y);
		return;
	}

	for (MenuItem& item : m_menuItems)
	{
		item.HandleEvent(event);
	}

	if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
	{
		for (std::size_t i = 0; i < m_menuItems.size(); ++i)
		{
			if (m_menuItems[i].ContainsPoint(event.button.x, event.button.y))
			{
				if (m_selectedIndex != i)
				{
					SetSelectedIndex(i);
				}

				ActivateSelectedItem();
				return;
			}
		}
	}
}

void Menu::Render(SDL_Renderer* renderer, TextManager& textManager) const
{
	for (const MenuItem& item : m_menuItems)
	{
		item.Render(renderer, textManager);
	}
}

void Menu::SelectFirstItem()
{
	if (m_menuItems.empty())
	{
		return;
	}

	SetSelectedIndex(0, false);
}

void Menu::SetOnSelectionChanged(Callback callback)
{
	m_onSelectionChanged = std::move(callback);
}

void Menu::SetOnItemActivated(Callback callback)
{
	m_onItemActivated = std::move(callback);
}

void Menu::UpdateSelection()
{
	for (std::size_t i = 0; i < m_menuItems.size(); i++)
	{
		m_menuItems[i].SetHovered(i == m_selectedIndex);
	}
}

void Menu::SelectNextItem()
{
	if (m_menuItems.empty())
	{
		return;
	}

	SetSelectedIndex((m_selectedIndex + 1) % m_menuItems.size());
}

void Menu::SelectPreviousItem()
{
	if (m_menuItems.empty())
	{
		return;
	}

	SetSelectedIndex((m_selectedIndex + m_menuItems.size() - 1) % m_menuItems.size());
}

void Menu::ActivateSelectedItem()
{
	if (m_menuItems.empty())
	{
		return;
	}

	if (m_onItemActivated)
	{
		m_onItemActivated();
	}

	m_menuItems[m_selectedIndex].Activate();
}

void Menu::SyncSelectionFromMouse(int mouseX, int mouseY)
{
	for (std::size_t i = 0; i < m_menuItems.size(); ++i)
	{
		if (m_menuItems[i].ContainsPoint(mouseX, mouseY))
		{
			if (m_selectedIndex != i)
			{
				SetSelectedIndex(i);
			}

			return;
		}
	}
}

void Menu::SetSelectedIndex(std::size_t index, bool notify)
{
	if (m_menuItems.empty() || index >= m_menuItems.size())
	{
		return;
	}

	const bool selectionChanged = m_selectedIndex != index;
	m_selectedIndex = index;

	UpdateSelection();

	if (selectionChanged && notify && m_onSelectionChanged)
	{
		m_onSelectionChanged();
	}
}