#pragma once

#include <vector>

namespace engine
{
    template <typename T, typename = std::is_default_constructible<T>>
    class SceneGraphNode
    {
    private:
        SceneGraphNode *_parent;
        std::vector<SceneGraphNode> _children;
        T _data;
        bool _is_dirty;

    public:
        SceneGraphNode() : SceneGraphNode(nullptr, T()) {}
        SceneGraphNode(SceneGraphNode *parent, const T &data) : _parent(parent), _data(data), _is_dirty(true) {}

        void set_data(const T &data)
        {
            _data = data;
            _is_dirty = true;
        }

        T &get_data()
        {
            return _data;
        }

        SceneGraphNode *get_parent()
        {
            return _parent;
        }

        SceneGraphNode &add_child(const T &data)
        {
            SceneGraphNode &child = add_child();
            child.set_data(data);

            return child;
        }

        SceneGraphNode &add_child()
        {
            SceneGraphNode child;
            child._parent = this;
            this->_children.push_back(child);
            this->_is_dirty = true;

            return this->_children.back();
        }
    };
} // namespace engine
