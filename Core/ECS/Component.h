#pragma once
#include "Entity.h"
#include <unordered_map>
#include <cassert>
#include <utility>

class IComponent
{
public:
	virtual ~IComponent() = default;
	virtual void Remove(Entity::EntityId entityid) = 0;
};

template<typename TComponent>
class Component : public IComponent
{
public:
	template<typename... TArgs>
	TComponent& Add(Entity::EntityId entityId, TArgs&&... args)
	{
		assert(m_components.find(entityId) == m_components.end() && "Entity already has this component.");

		auto [it, inserted] = m_components.try_emplace(entityId, std::forward<TArgs>(args)...);

		assert(inserted && "Failed to add component to entity.");

		return it->second;
	}

	TComponent* Get(Entity::EntityId entityId)
	{
		auto it = m_components.find(entityId);
		
		if (it != m_components.end())
		{
			return &it->second;
		}
		
		return nullptr;
	}

	const TComponent* Get(Entity::EntityId entityId) const
	{
		auto it = m_components.find(entityId);

		if (it != m_components.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	bool Has(Entity::EntityId entityId) const
	{
		return m_components.find(entityId) != m_components.end();
	}

	void Remove(Entity::EntityId entityId) override
	{
		m_components.erase(entityId);
	}
	
private:
	std::unordered_map<Entity::EntityId, TComponent> m_components;
};