#include "scene/components/inputcomponent.hpp"

namespace engine {
    InputComponent::InputComponent(const std::vector<int> keys) {
        for (int key : keys) {
            this->keys[key] = {false, false};
        }
    }
}