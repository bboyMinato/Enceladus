#pragma once
#include "Entity.h"
#include "Component.h"
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <memory>
#include <cassert>
#include <utility>
#include <functional>

class Registry final
{
public:
	Entity CreateEntity()
	{
		const Entity::EntityId newId = ++m_nextEntityId;
		
		m_entities.insert(newId);
		
		return Entity(newId, *this);
	}

	void DestroyEntity(Entity entity)
	{
		assert(entity.IsValid() && &entity.GetRegistry() == this && "Entity does not belong to this registry.");
		
		const Entity::EntityId entityId = entity.GetId();

		if (m_entities.find(entityId) == m_entities.end())
		{
			return; // Entity does not exist
		}

		for (auto& [typeIndex, component] : m_components)
		{
			(void)typeIndex; // Suppress unused variable warning
			component->Remove(entityId);
		}

		m_entities.erase(entityId);
	}

	bool IsValid(Entity entity) const
	{		
		return entity.IsValid() &&
			   &entity.GetRegistry() == this &&
			   m_entities.find(entity.GetId()) != m_entities.end();
	}

	template<typename TComponent>
	TComponent* GetComponent(Entity entity)
	{
		assert(IsValid(entity) && "Entity is not valid.");

		Component<TComponent>* component = GetComponentStorage<TComponent>();

		if (component)
		{
			return component->Get(entity.GetId());
		}

		return nullptr;
	}

	template<typename TComponent, typename... TArgs>
	TComponent& AddComponent(Entity entity, TArgs&&... args)
	{
		assert(IsValid(entity) && "Entity is not valid.");
		
		return GetOrCreateStorage<TComponent>().Add(entity.GetId(), std::forward<TArgs>(args)...);
	}

	template<typename TComponent>
	const TComponent* GetComponent(Entity entity) const
	{
		assert(IsValid(entity) && "Entity is not valid.");

		const Component<TComponent>* component = GetComponentStorage<TComponent>();

		if (component)
		{
			return component->Get(entity.GetId());
		}

		return nullptr;
	}

	template<typename TComponent>
	bool HasComponent(Entity entity) const
	{
		assert(IsValid(entity) && "Entity is not valid.");
		
		const Component<TComponent>* component = GetComponentStorage<TComponent>();
		
		if (!component)
		{
			return false;
		}

		return component->Has(entity.GetId());
	}

	template<typename TComponent>
	void RemoveComponent(Entity entity)
	{
		assert(IsValid(entity) && "Entity is not valid.");
		
		Component<TComponent>* component = GetComponentStorage<TComponent>();
		
		if (component)
		{
			component->Remove(entity.GetId());
		}
	}

	template<typename... TComponents, typename TFunc>
	void ForEach(TFunc&& func)
	{
		for (const Entity::EntityId entityId : m_entities)
		{
			Entity entity(entityId, *this);

			if ((HasComponent<TComponents>(entity) && ...))
			{
				func(entity, *GetComponent<TComponents>(entity)...);
			}
		}
	}
	
private:
	template<typename TComponent>
	Component<TComponent>& GetOrCreateStorage()
	{
		const std::type_index typeIndex(typeid(TComponent));
		auto it = m_components.find(typeIndex);
		if (it == m_components.end())
		{
			auto componentStorage = std::make_unique<Component<TComponent>>();
			Component<TComponent>* componentStoragePtr = componentStorage.get();

			m_components.emplace(typeIndex, std::move(componentStorage));

			return *componentStoragePtr;
		}

		return *static_cast<Component<TComponent>*>(it->second.get());
	}

	template<typename TComponent>
	Component<TComponent>* GetComponentStorage()
	{
		const std::type_index typeIndex(typeid(TComponent));
		auto it = m_components.find(typeIndex);

		if (it == m_components.end())
		{
			return nullptr;
		}
		return static_cast<Component<TComponent>*>(it->second.get());
	}

	template<typename TComponent>
	const Component<TComponent>* GetComponentStorage() const
	{
		const std::type_index typeIndex(typeid(TComponent));
		auto it = m_components.find(typeIndex);

		if (it == m_components.end())
		{
			return nullptr;
		}

		return static_cast<const Component<TComponent>*>(it->second.get());
	}

private:
	Entity::EntityId m_nextEntityId = 0;

	std::unordered_set<Entity::EntityId> m_entities;
	std::unordered_map<std::type_index, std::unique_ptr<IComponent>> m_components;
};

// Entity functions that forward to the registry
// These functions are defined here to avoid circular dependencies between Entity and Registry
template<typename TComponent, typename... TArgs>
TComponent& Entity::Add(TArgs&&... args)
{
	assert(IsValid() && "Entity is not valid.");
	return m_registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template<typename TComponent>
TComponent* Entity::Get()
{
	assert(IsValid() && "Entity is not valid.");
	return m_registry->GetComponent<TComponent>(*this);
}

template<typename TComponent>
const TComponent* Entity::Get() const
{
	assert(IsValid() && "Entity is not valid.");
	return m_registry->GetComponent<TComponent>(*this);
}

template<typename TComponent>
bool Entity::Has() const
{
	assert(IsValid() && "Entity is not valid.");
	return m_registry->HasComponent<TComponent>(*this);
}

template<typename TComponent>
void Entity::Remove()
{
	assert(IsValid() && "Entity is not valid.");
	m_registry->RemoveComponent<TComponent>(*this);
}

inline void Entity::Destroy()
{
	assert(IsValid() && "Entity is not valid.");
	m_registry->DestroyEntity(*this);
}