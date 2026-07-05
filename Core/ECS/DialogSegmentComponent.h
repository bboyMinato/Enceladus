#pragma once
#include <string>

struct DialogSegmentComponent {
    DialogSegmentComponent(const std::string &name) : name(name), pos(0), part(0) {}

    std::string name;
    std::size_t pos;
    std::size_t part;
    std::string text;
};
