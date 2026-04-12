#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <utility>
#include <concepts>
#include "../States/IState.h"

class Engine;

class StateManager
{
public:
	explicit StateManager(Engine& engine);

	template<typename TState, typename... TArgs>
	void PushState(TArgs&&... args)
	{
		static_assert(std::derived_from<TState, IGameState>, "TState must derive from IGameState");

		std::unique_ptr<IGameState> state = std::make_unique<TState>(std::forward<TArgs>(args)...);
		m_pendingStateChanges.push_back(
			StateCommand{
				StateCommandType::Push,
				std::move(state)
			});
	}

	template<typename TState, typename... TArgs>
	void ReplaceState(TArgs&&... args)
	{
		static_assert(std::derived_from<TState, IGameState>, "TState must derive from IGameState");

		std::unique_ptr<IGameState> state = std::make_unique<TState>(std::forward<TArgs>(args)...);
		m_pendingStateChanges.push_back(
			StateCommand{
				StateCommandType::Replace,
				std::move(state)
			});
	}

	void Pop();
	void Clear(); 
	void ApplyPendingStateChanges(Engine& engine);

	void HandleEvent(Engine& engine, const SDL_Event& event);
	void Update(Engine& engine, float deltaTime);
	void Render(Engine& engine, SDL_Renderer* renderer);

	inline bool HasState() const { return !m_stateStack.empty(); }

private:
	enum class StateCommandType
	{
		Push,
		Pop,
		Clear,
		Replace
	};

	struct StateCommand
	{
		StateCommandType type;
		std::unique_ptr<IGameState> state;
	};

	std::vector<std::unique_ptr<IGameState>> m_stateStack;
	std::vector<StateCommand> m_pendingStateChanges;
};