#pragma once
#include <string>

struct DialogSegmentComponent
{
	DialogSegmentComponent(const std::string& name) : name(name), part(0) {}

	std::string name;
	int part;
	std::string text;
};
