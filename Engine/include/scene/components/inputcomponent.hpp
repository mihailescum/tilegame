#pragma once

#include <unordered_map>
#include <vector>
#include <utility>

namespace engine
{
    struct InputComponent
    {
        public:
        InputComponent() = default;
        InputComponent(const InputComponent &input) = default;
        InputComponent(const std::vector<int> keys);

        std::unordered_map<int, std::pair<bool, bool>> keys;
    };
} // namespace engine