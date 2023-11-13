#pragma once

#include <vector>
#include <memory>

namespace engine
{
    template <typename T, typename = std::is_default_constructible<T>>
    class SceneGraphNode
    {
    private:
        SceneGraphNode *_parent;
        std::vector<std::unique_ptr<SceneGraphNode>> _children;
        T _data;
        bool _is_dirty;

    public:
        SceneGraphNode() : SceneGraphNode<T>(nullptr, T()) {}
        SceneGraphNode(SceneGraphNode<T> *parent, const T &data) : _parent(parent), _data(data), _is_dirty(true), _children() {}

        void data(const T &data)
        {
            _data = data;
            _is_dirty = true;
        }

        T &data() { return _data; }
        const T &data() const { return _data; }

        // TODO constness
        const SceneGraphNode *parent() const { return _parent; }
        SceneGraphNode *parent() { return _parent; }

        SceneGraphNode &add_child(const T &data)
        {
            SceneGraphNode &child = add_child();
            child.data(data);

            return child;
        }

        SceneGraphNode &add_child()
        {
            this->_children.push_back(std::make_unique<SceneGraphNode>(this, T()));
            this->_is_dirty = true;

            return *this->_children.back().get();
        }

        std::vector<std::unique_ptr<SceneGraphNode>> &children() { return _children; }
        const std::vector<std::unique_ptr<SceneGraphNode>> &children() const { return _children; }
    };
} // namespace engine
