#pragma once
#include "Button.h"
#include "../Managers/TextManager.h"
#include <functional>
#include <string>
#include <vector>

class MenuItem final
{
public:
	using Action = std::function<void()>;

	MenuItem(const SDL_Rect& bounds, Action action = {});

	void SetAction(Action action);
	void SetTextName(const std::string& textName);
	void HandleEvent(const SDL_Event& event);
	void Render(SDL_Renderer* renderer, TextManager& textManager) const;
	void SetHovered(bool isHovered);
	void Activate();
	bool IsHovered() const;
	bool ContainsPoint(int x, int y) const;

private:
	Button m_button;
	Action m_action;
};

class Menu final
{
public:
	using Callback = std::function<void()>;

	void AddMenuItem(const SDL_Rect& bounds, const std::string& textName, MenuItem::Action action = {});
	void HandleEvent(const SDL_Event& event);
	void Render(SDL_Renderer* renderer, TextManager& textManager) const;
	void Clear();
	void SelectFirstItem();
	void SetOnSelectionChanged(Callback callback);
	void SetOnItemActivated(Callback callback);

	inline bool IsEmpty() const { return m_menuItems.empty(); }

private:
	void UpdateSelection();
	void SelectNextItem();
	void SelectPreviousItem();
	void ActivateSelectedItem();
	void SyncSelectionFromMouse(int mouseX, int mouseY);
	void SetSelectedIndex(std::size_t index, bool notify = true);

private:
	std::vector<MenuItem> m_menuItems;
	std::size_t m_selectedIndex{ 0 };

	Callback m_onSelectionChanged;
	Callback m_onItemActivated;
};