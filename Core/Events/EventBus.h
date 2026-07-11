#pragma once
#include <vector>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include "../Events/Event.h"

class EventBus
{
public: 
	template<typename EventT>
	void Subscribe(std::function<void(const EventT&)> handler)
	{
		auto wrapper = [handler](const void* event)
			{
				handler(*static_cast<const EventT*>(event));
			};

		m_handlers[std::type_index(typeid(EventT))].push_back(std::move(wrapper));
	}

	template<typename EventT>
	void Emit(const EventT& event)
	{
		auto it = m_handlers.find(std::type_index(typeid(EventT)));
		if (it != m_handlers.end())
		{
			for (const auto& handler : it->second)
			{
				handler(&event);
			}
		}
	}	

private:
	std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> m_handlers;
};