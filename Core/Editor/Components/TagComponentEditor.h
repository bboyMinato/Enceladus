#pragma once
#include "../../ECS/Entity.h"
#include <array>

class TagComponentEditor
{
public:
	bool Render(Entity& entity);

private:
	Entity m_tagBufferEntity;
	std::array<char, 256> m_tagBuffer{};
};