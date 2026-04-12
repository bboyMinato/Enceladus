#pragma once
#include <cstdint>
#include <cassert>
#include <utility>

class Registry;

class Entity
{
public:
	using EntityId = std::uint32_t;

	Entity() = default;
	Entity(EntityId id, Registry& registry)
		: m_id(id), m_registry(&registry)
	{
	}

	template<typename TComponent, typename... TArgs>
	TComponent& Add(TArgs&&... args);

	template<typename TComponent>
	TComponent* Get();

	template<typename TComponent>
	const TComponent* Get() const;

	template<typename TComponent>
	bool Has() const;

	template<typename TComponent>
	void Remove();

	void Destroy();

	inline EntityId GetId() const { return m_id; }

	inline Registry& GetRegistry() const
	{
		assert(m_registry != nullptr && "Entity has no owning registry.");
		return *m_registry;
	}

	inline bool IsValid() const
	{
		return m_registry != nullptr && m_id != 0;
	}

	inline explicit operator bool() const
	{
		return IsValid();
	}

	inline bool operator==(const Entity& other) const
	{
		return m_id == other.m_id && m_registry == other.m_registry;
	}

	inline bool operator!=(const Entity& other) const
	{
		return !(*this == other);
	}

private:
	EntityId m_id{ 0 };
	Registry* m_registry{ nullptr };
};