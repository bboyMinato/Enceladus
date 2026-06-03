#include <string>

class TagComponent
{
public:
	std::string Tag;
	TagComponent() = default;
	TagComponent(const std::string& tag) : Tag(tag) {}
};