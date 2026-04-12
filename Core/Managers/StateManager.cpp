#include "StateManager.h"

StateManager::StateManager(Engine& engine)
{
}

void StateManager::Pop()
{
	m_pendingStateChanges.push_back(StateCommand{ StateCommandType::Pop, nullptr });
}

void StateManager::Clear()
{
	m_pendingStateChanges.push_back(StateCommand{ StateCommandType::Clear, nullptr });
}

void StateManager::ApplyPendingStateChanges(Engine& engine)
{
	for (auto& command : m_pendingStateChanges)
	{
		switch (command.type)
		{
		case StateCommandType::Push:
			if (!command.state)
			{
				continue;
			}

			if (HasState())
			{
				m_stateStack.back()->OnPause(engine);
			}
			m_stateStack.push_back(std::move(command.state));
			m_stateStack.back()->OnEnter(engine);
			break;

		case StateCommandType::Pop:
			if (HasState())
			{
				m_stateStack.back()->OnExit(engine);
				m_stateStack.pop_back();
			}

			if (HasState())
			{
				m_stateStack.back()->OnResume(engine);
			}
			break;

		case StateCommandType::Clear:
			while (HasState())
			{
				m_stateStack.back()->OnExit(engine);
				m_stateStack.pop_back();
			}
			break;
		
		case StateCommandType::Replace:
			if (!command.state)
			{
				continue;
			}

			if (HasState())
			{
				m_stateStack.back()->OnExit(engine);
				m_stateStack.pop_back();
			}

			m_stateStack.push_back(std::move(command.state));
			m_stateStack.back()->OnEnter(engine);
			break;
		}		
	}

	m_pendingStateChanges.clear();
}

void StateManager::HandleEvent(Engine& engine, const SDL_Event& event)
{
	if (HasState())
	{
		m_stateStack.back()->HandleEvent(engine, event);
	}
}

void StateManager::Update(Engine& engine, float deltaTime)
{
	if (HasState())
	{
		m_stateStack.back()->Update(engine, deltaTime);
	}
}

void StateManager::Render(Engine& engine, SDL_Renderer* renderer)
{
	if (HasState())
	{
		m_stateStack.back()->Render(engine, renderer);
	}
}