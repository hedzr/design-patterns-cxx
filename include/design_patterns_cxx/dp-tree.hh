// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/20.
//

#ifndef DESIGN_PATTERNS_CXX_DP_TREE_HH
#define DESIGN_PATTERNS_CXX_DP_TREE_HH

#include <cstddef>
#include <cstring>

#include <deque>
#include <queue>
#include <tuple>
#include <vector>

#include <algorithm>
#include <functional>
#include <memory>

#include <cassert>

namespace dp::tree::detail {

    template<typename Node>
    class tree_ops {
    public:
        using size_type = std::size_t;
        using NodePtr = Node *;

    protected:
        size_type calc_height(NodePtr node) const {
            if (node == nullptr)
                return 0;
            int left_side;
            int right_side;
            left_side = calc_height(node->left);
            right_side = calc_height(node->right);
            return (left_side > right_side) ? left_side + 1 : right_side + 1;
        }
        size_type calc_count(NodePtr node) const {
            if (node == nullptr)
                return 0;
            size_type n{1};
            n += calc_count(node->left);
            n += calc_count(node->right);
            return n;
        }

    public:
        // Depth First Traversals
        void traverse_in_order(NodePtr node, std::function<void(NodePtr)> &&fn) {
            if (node == nullptr)
                return;

            traverse_in_order(node->left, std::move(fn));
            fn(node);
            traverse_in_order(node->right, std::move(fn));
        }
        // Depth First Traversals
        void traverse_in_order_rev(NodePtr node, std::function<void(NodePtr)> &&fn) {
            if (node == nullptr)
                return;

            traverse_in_order(node->right, std::move(fn));
            fn(node);
            traverse_in_order(node->left, std::move(fn));
        }
        // Depth First Traversals
        void traverse_pre_order(NodePtr node, std::function<void(NodePtr)> &&fn) {
            if (node == nullptr)
                return;

            fn(node);
            traverse_pre_order(node->left, std::move(fn));
            traverse_pre_order(node->right, std::move(fn));
        }
        // Depth First Traversals
        void traverse_post_order(NodePtr node, std::function<void(NodePtr)> &&fn) {
            if (node == nullptr)
                return;

            traverse_post_order(node->left, std::move(fn));
            traverse_post_order(node->right, std::move(fn));
            fn(node);
        }
        // Breadth First Traversals
        void traverse_level_order(NodePtr node, std::function<void(NodePtr)> &&fn) {
            for (int h = calc_height(node), i = 1; i <= h; i++)
                traverse_current_level(node, i, std::move(fn));
        }

    protected:
        void traverse_current_level(NodePtr node, int level, std::function<void(NodePtr)> &&fn) {
            if (node == nullptr)
                return;
            if (level == 1)
                fn(node);
            else if (level > 1) {
                traverse_current_level(node->left, level - 1, std::move(fn));
                traverse_current_level(node->right, level - 1, std::move(fn));
            }
        }
    };

    template<typename Data>
    struct rb_node_t {
        Data key{};
        bool color_is_red{};
        rb_node_t *left{};
        rb_node_t *right{};
        rb_node_t *parent{};

        // rb_node_t() {}
        ~rb_node_t() {
            if (left) delete left;
            if (right) delete right;
        }
    };

} // namespace dp::tree::detail


// --------------------------------------- rb_tree
namespace dp::tree {

    template<typename Data, typename Node = detail::rb_node_t<Data>>
    class rb_tree : public detail::tree_ops<Node> {
    public:
        rb_tree() {}
        ~rb_tree() { clear(); }

        using Base = detail::tree_ops<Node>;
        using size_type = typename Base::size_type;
        using TreePtr = typename Base::NodePtr;
        using NodePtr = typename Base::NodePtr;

    private:
        NodePtr _root{nullptr};

    public:
        void clear() {
            if (_root) {
                delete _root;
                _root = nullptr;
            }
        }

        NodePtr root() { return _root; }
        const NodePtr root() const { return _root; }
        size_type height() const { return Base::calc_height(_root); }
        size_type count() const { return Base::calc_count(_root); }

        void insert(Data const &v) {
            if (_root == nullptr) {
                _root = new Node{v};
                return;
            }
            auto *node = new Node{v};
            insert_rbt(_root, node);
        }
        void insert(Data &&v) {
            if (_root == nullptr) {
                _root = new Node{std::move(v)};
                return;
            }
            auto *node = new Node{std::move(v)};
            insert_rbt(_root, node);
        }
        template<typename... Args>
        void emplace(Args &&...args) {
            if (_root == nullptr) {
                _root = new Node{std::move(Data{args...})};
                return;
            }
            auto *node = new Node{std::move(Data{args...})};
            insert_rbt(_root, node);
        }

    private:
        int insert_rbt(NodePtr &root, NodePtr node);
        int insert_rbt_fixup(NodePtr &root, NodePtr node);
        static NodePtr rbt_rotate_right(NodePtr &root, NodePtr node);
        static NodePtr rbt_rotate_left(NodePtr &root, NodePtr node);

    public:
        bool erase(int v) {
            if (_root)
                return delete_rbt(_root, v) == 0;
            return false;
        }

    private:
        int delete_rbt(NodePtr &root, int key);
        static int delete_rbt_fixup(NodePtr &root, NodePtr node, NodePtr parent);
    };

} // namespace dp::tree

// --------------------------------------- rb_tree inline objs
namespace dp::tree {

    template<typename Data, typename Node>
    inline int rb_tree<Data, Node>::insert_rbt(NodePtr &root, NodePtr node) {
        NodePtr p = root, last = nullptr;

        //find the position we need to insert
        while (p) {
            last = p;
            if (p->key == node->key)
                return 1;
            p = (p->key < node->key) ? p->right : p->left;
        }

        node->parent = last;
        node->color_is_red = true;
        if (last != nullptr) {
            if (node->key < last->key)
                last->left = node;
            else
                last->right = node;
        } else {
            root = node;
        }

        return insert_rbt_fixup(root, node);
    }

    template<typename Data, typename Node>
    inline int rb_tree<Data, Node>::insert_rbt_fixup(NodePtr &root, NodePtr node) {
        NodePtr parent;
        NodePtr grandpa;

        // If parent exist, and the color of parent is RED
        while ((parent = node->parent) && parent->color_is_red) {
            grandpa = parent->parent;

            // parent node is grandpa node's left child (grandpa shouldn't be NULL since parent->color_is_red)
            if (grandpa->left == parent) {
                NodePtr uncle = grandpa->right;

                // Case 1: uncle is RED
                if (uncle && uncle->color_is_red) {
                    parent->color_is_red = false;
                    uncle->color_is_red = false;
                    grandpa->color_is_red = true;

                    node = grandpa;
                    continue;
                }

                // Case 2: uncle is BLACK, and node is parent's right child
                if (parent->right == node) {
                    rbt_rotate_left(root, parent);

                    // reset parent and node pointer
                    auto *tmp = parent;
                    parent = node, node = tmp;

                    // Here successful convert Case 2 to Case3
                }

                // Case 3: uncle is BLACK, and node is parent's left child
                parent->color_is_red = false;
                grandpa->color_is_red = true;
                rbt_rotate_right(root, grandpa);

            } else {
                NodePtr uncle = grandpa->left;

                // Case 1: uncle is RED
                if (uncle && uncle->color_is_red) {
                    parent->color_is_red = false;
                    uncle->color_is_red = false;
                    grandpa->color_is_red = true;

                    node = grandpa;
                    continue;
                }

                // Case 2: uncle is BLACK, and node is parent's left child
                if (parent->left == node) {
                    rbt_rotate_right(root, parent);

                    //reset parent and node pointer
                    auto *tmp = parent;
                    parent = node, node = tmp;

                    //Here success convert Case 2 to Case 3
                }

                // Case 3: uncle is BLACK, and node is parent's right child
                parent->color_is_red = false;
                grandpa->color_is_red = true;
                rbt_rotate_left(root, grandpa);
            }
        }

        root->color_is_red = false;
        return 0;
    }

    //          d                   b
    //       b      f     ->     a     d
    //     a   c                     c   f
    template<typename Data, typename Node>
    inline typename rb_tree<Data, Node>::NodePtr rb_tree<Data, Node>::rbt_rotate_right(NodePtr &root, NodePtr node) {
        NodePtr left = node->left;
        left->parent = node->parent;
        if (node->parent) {
            if (node->parent->left == node)
                node->parent->left = left;
            else
                node->parent->right = left;
        } else {
            root = left;
        }

        node->parent = left;
        node->left = left->right;
        left->right = node;
        return left;
    }

    //          d                   f
    //       b      f     ->     d     g
    //             e  g        b   e
    //
    template<typename Data, typename Node>
    inline typename rb_tree<Data, Node>::NodePtr rb_tree<Data, Node>::rbt_rotate_left(NodePtr &root, NodePtr node) {
        NodePtr right = node->right;

        right->parent = node->parent;
        if (node->parent) {
            if (node->parent->left == node)
                node->parent->left = right;
            else
                node->parent->right = right;
        } else {
            root = right;
        }

        node->parent = right;
        node->right = right->left;
        right->left = node;
        return right;
    }

    template<typename Data, typename Node>
    inline int rb_tree<Data, Node>::delete_rbt(NodePtr &root, int key) {
        NodePtr p = root;
        while (p) {
            if (p->key == key)
                break;
            p = (p->key < key) ? p->right : p->left;
        }

        if (!p)
            return -1;

        if (p->left && p->right) {
            NodePtr successor = p->right;

            while (successor->left)
                successor = successor->left;

            if (p->parent) {
                if (p->parent->left == p)
                    p->parent->left = successor;
                else
                    p->parent->right = successor;
            } else {
                root = successor;
            }

            auto successor_child = successor->right;
            auto successor_parent = successor->parent;
            auto color_is_red = successor->color_is_red;

            if (successor_parent == p) {
                successor_parent = successor;
            } else {
                if (successor_child)
                    successor_child->parent = successor_parent;

                successor_parent->left = successor_child;

                successor->right = p->right;
                p->right->parent = successor;
            }

            successor->parent = p->parent;
            successor->color_is_red = p->color_is_red;
            successor->left = p->left;
            p->left->parent = successor;

            if (!color_is_red)
                delete_rbt_fixup(root, successor_child, successor_parent);

            p->left = nullptr;
            p->right = nullptr;
            delete (p);
            return 0;
        }

        auto child = (p->left) ? p->left : p->right;
        auto parent = p->parent;
        auto color_is_red = p->color_is_red;

        if (child)
            child->parent = parent;

        if (parent) {
            if (parent->left == p)
                parent->left = child;
            else
                parent->right = child;
        } else {
            root = child;
        }

        if (!color_is_red)
            delete_rbt_fixup(root, child, parent);

        p->left = p->right = nullptr;
        delete (p);
        return 0;
    }

    template<typename Data, typename Node>
    inline int rb_tree<Data, Node>::delete_rbt_fixup(NodePtr &root, NodePtr node, NodePtr parent) {
        NodePtr brother = nullptr;

        while ((!node || !node->color_is_red) && node != root) {
            if (parent->left == node) {
                // left branch

                // brother can't be NULL, because we have deleted a black node,
                // and the tree is balanced before we delete the node
                brother = parent->right;

                if (brother->color_is_red) {
                    // Case 1: x's brother is COLOR_RED
                    brother->color_is_red = false;
                    parent->color_is_red = true;
                    rbt_rotate_left(root, node);
                    brother = parent->right;
                }

                if ((!brother->left || !brother->left->color_is_red) &&
                    (!brother->right || !brother->right->color_is_red)) {
                    // Case 2: x's brother is BLACK, is its two child is NULL or BLACK
                    brother->color_is_red = true;
                    node = parent;
                    parent = node->parent;
                } else {
                    if (!brother->right || !brother->right->color_is_red) {
                        // Case 3: x's brother is BLACK, and brother left child is RED,
                        // right child is BLACK
                        brother->left->color_is_red = false;
                        brother->color_is_red = true;
                        rbt_rotate_right(root, brother);
                        brother = parent->right;
                    }

                    // Case 4: x's brother is BLACK, and brother's right child is RED
                    brother->color_is_red = parent->color_is_red;
                    parent->color_is_red = false;
                    brother->right->color_is_red = false;
                    rbt_rotate_left(root, parent);

                    node = root;
                    break;
                }

            } else {
                // right branch
                brother = parent->left;

                if (brother->color_is_red) {
                    // Case 1: x's brother is RED
                    brother->color_is_red = false;
                    parent->color_is_red = true;
                    rbt_rotate_right(root, parent);
                    brother = parent->left;
                }

                if ((!brother->left || !brother->left->color_is_red) &&
                    (!brother->right || !brother->right->color_is_red)) {
                    // Case 2: x's brother is BLACK, is its two child is NULL or BLACK
                    brother->color_is_red = true;
                    node = parent;
                    parent = node->parent;
                } else {
                    if (!brother->left || !brother->left->color_is_red) {
                        // Case 3: x's brother is LACK, and brother right child is RED,
                        // left child is BLACK
                        brother->right->color_is_red = false;
                        brother->color_is_red = true;
                        rbt_rotate_left(root, brother);
                        brother = parent->left;
                    }

                    // Case 4: x's brother is BLACK, and brother's left child is RED
                    brother->color_is_red = parent->color_is_red;
                    parent->color_is_red = false;
                    brother->left->color_is_red = false;
                    rbt_rotate_right(root, parent);

                    node = root;
                    break;
                }
            }
        }

        if (node) {
            node->color_is_red = false;
        }
        return 0;
    }

} // namespace dp::tree


// --------------------------------------- btree
namespace dp::tree {

    class btree__t__ {
    public:
    };


} // namespace dp::tree

// --------------------------------------- tree_t
namespace dp::tree {

    using size_type = std::size_t;

    namespace detail {

        template<typename Node>
        class generic_tree_ops {
        public:
            using NodePtr = Node *;
            // using NodeUP = std::unique_ptr<Node>;
            using Nodes = std::vector<NodePtr>;

        public:
            virtual void clear() {}
            virtual size_type count() const { return 0; }

        public:
            static void traverse_tree(NodePtr node, std::function<void(NodePtr const)> &&fn) {
                if (node == nullptr)
                    return;
                for (auto &n : node->children)
                    traverse(n, std::move(fn));
                fn(node);
            }
        };

        template<typename Data>
        struct generic_node_t {
            using Node = generic_node_t<Data>;
            using NodePtr = Node *; //std::unique_ptr<Node>;
            using Nodes = std::vector<NodePtr>;

        private:
            Data _data{};
            NodePtr _parent{nullptr};
            Nodes _children{};

        public:
            generic_node_t() {}
            generic_node_t(Data const &data_)
                : _data{data_} {}
            generic_node_t(Data &&data_)
                : _data{std::move(data_)} {}
            template<typename... Args>
            generic_node_t(Args &&...args)
                : _data{std::forward<Args>(args)...} {}
            ~generic_node_t() { clear(); }

            // operator Data() { return _data; }
            Data &operator*() { return _data; }
            Data const &operator*() const { return _data; }
            NodePtr const parent() const { return _parent; }

        public:
            void clear() {
                for (auto *it : _children) {
                    if (it)
                        delete it;
                }
                _children.clear();
            }

        public:
            void insert(Data const &val) {
                auto item = new Node{val}; //std::make_unique<Node>(val);
                item->_parent = this;
                _children.push_back(std::move(item));
            }
            void insert(Data &&val) {
                auto item = new Node{val}; //std::make_unique<Node>(val);
                item->_parent = this;
                _children.push_back(std::move(item));
                // children.emplace_back(val);
            }
            template<typename... Args>
            void emplace(Args &&...args) {
                auto item = new Node{std::forward<Args>(args)...}; // std::make_unique<Node>(std::forward<Args>(args)...);
                item->_parent = this;
                _children.push_back(std::move(item));
                // children.emplace_back(std::forward<Args>(args)...);
            }
            void erase(size_type index) {
                _children.erase(index);
            }

        public:
            /**
             * @brief preorder = parent -> left -> right. preorder_iter walks the whole tree with parent first and children followed.
             */
            struct preorder_iter_data {

                // iterator traits
                using difference_type = std::ptrdiff_t;
                using value_type = Node;
                using pointer = value_type *;
                using reference = value_type &;
                using iterator_category = std::forward_iterator_tag;
                using self = preorder_iter_data;

                preorder_iter_data() {}
                preorder_iter_data(pointer ptr_, bool on_children_ = false, size_type idx_ = 0)
                    : _ptr(ptr_)
                    , _on_children(on_children_)
                    , _child_idx(idx_) {}
                preorder_iter_data(const preorder_iter_data &o)
                    : _ptr(o._ptr)
                    , _on_children(o._on_children)
                    , _child_idx(o._child_idx) {}
                preorder_iter_data &operator=(const preorder_iter_data &o) {
                    _ptr = o._ptr, _on_children = o._on_children, _child_idx = o._child_idx;
                    return *this;
                }

                bool operator==(self const &r) const { return _ptr == r._ptr && _on_children == r._on_children && _child_idx == r._child_idx; }
                bool operator!=(self const &r) const { return _ptr != r._ptr || _on_children != r._on_children || _child_idx != r._child_idx; }
#if 0
                bool operator==(preorder_iter_data const &r) const {
                    return _ptr == r._ptr && _on_children == r._on_children && _child_idx == r._child_idx;
                    // if (_ptr == r._ptr) {
                    //     if (_ptr == nullptr)
                    //         return true;
                    //     if (_on_children == r._on_children)
                    //         return _child_idx == r._child_idx;
                    //     if (!_on_children && _child_idx == 0 && r._on_children && r._child_idx == r._ptr->_children.size())
                    //         return true;
                    //     if (!r._on_children && r._child_idx == 0 && _on_children && _child_idx == _ptr->_children.size())
                    //         return true;
                    // }
                    // return false;
                }
                bool operator!=(preorder_iter_data const &r) const { return !operator==(r); }
#endif
                reference operator*() const {
                    auto &ret = _ptr;
                    if (_on_children) {
                        if (ret->_children.empty() || _child_idx >= ret->_children.size())
                            return *ret;
                        return *(ret->_children[_child_idx]);
                    }
                    return *ret;
                }
                self &operator++() { return _incr(); }
                self operator++(int) {
                    self copy{_ptr, _on_children, _child_idx};
                    ++(*this);
                    return copy;
                }

                static self begin(pointer root_) {
                    return self{root_};
                    // NodePtr p = root_, last{nullptr};
                    // while (p) {
                    //     last = p;
                    //     if (p->_children.empty())
                    //         break;
                    //     p = (p->_children[0]);
                    // }
                    // return self{last, !last->_children.empty()};
                }
                static self end(pointer root_) {
                    if (root_ == nullptr) return self{root_};
                    pointer p = root_, last{nullptr};
                    while (p) {
                        last = p;
                        if (p->_children.empty())
                            break;
                        p = (p->_children[p->_children.size() - 1]);
                    }
                    auto it = self{last, true};
                    ++it;
                    return it;
                }

            private:
                self &_incr() {
                    auto ret = _ptr;
                    // auto idx = _child_idx;

                    if (_on_children) {
                        _child_idx++;
                    } else {
                        _on_children = true;
                        return (*this);
                    }

                    if (ret->_parent && _child_idx >= ret->_children.size()) {
                        _ptr = ret->_parent;
                        _child_idx = 0;
                        _on_children = false;

                        for (auto *it : ret->_parent->_children) {
                            _child_idx++;
                            if (it == ret) break;
                        }

                        if (_child_idx >= ret->_parent->_children.size()) {
                            // end() ?
                            _on_children = true;
                        }
                    }

                    return (*this);
                }

                pointer _ptr{};
                bool _on_children{};
                size_type _child_idx{};
            };

            using iterator = preorder_iter_data;
            using const_iterator = const iterator;
            iterator begin() { return iterator::begin(this); }
            const_iterator begin() const { return const_iterator::begin(this); }
            iterator end() { return iterator::end(this); }
            const_iterator end() const { return const_iterator::end(this); }

            struct rev_preorder_iter_data {

                // iterator traits
                using difference_type = std::ptrdiff_t;
                using value_type = Node;
                using pointer = value_type *;
                using reference = value_type &;
                using iterator_category = std::forward_iterator_tag;
                using self = rev_preorder_iter_data;

                rev_preorder_iter_data() {}
                rev_preorder_iter_data(pointer ptr_, bool on_children_ = false, size_type idx_ = 0)
                    : _ptr(ptr_)
                    , _on_children(on_children_)
                    , _child_idx(idx_) {}
                rev_preorder_iter_data(const rev_preorder_iter_data &o)
                    : _ptr(o._ptr)
                    , _on_children(o._on_children)
                    , _child_idx(o._child_idx) {}
                rev_preorder_iter_data &operator=(const rev_preorder_iter_data &o) {
                    _ptr = o._ptr, _on_children = o._on_children, _child_idx = o._child_idx;
                    return *this;
                }

                bool operator==(self const &r) const { return _ptr == r._ptr && _on_children == r._on_children && _child_idx == r._child_idx; }
                bool operator!=(self const &r) const { return _ptr != r._ptr || _on_children != r._on_children || _child_idx != r._child_idx; }
                reference operator*() const {
                    auto &ret = _ptr;
                    if (_on_children) {
                        if (ret->_children.empty() || _child_idx >= ret->_children.size())
                            return *ret;
                        return *(ret->_children[_child_idx]);
                    }
                    return *ret;
                }
                self &operator++() { return _decr(); }
                self operator++(int) {
                    self copy{_ptr, _on_children, _child_idx};
                    ++(*this);
                    return copy;
                }
                static self begin(pointer root_) {
                    if (root_) {
                        pointer p = root_, last{nullptr};
                        while (p) {
                            last = p;
                            if (p->_children.empty())
                                break;
                            p = (p->_children[p->_children.size() - 1]);
                        }
                        return self{last};
                    }
                    return self{root_};
                }
                static self end(pointer) {
                    return self{nullptr};
                }

            private:
                self &_decr() {
                    auto ret = _ptr;
                    auto idx = _child_idx;

                    if (_on_children) {
                        if (!(ret->_children.empty() || _child_idx <= 0)) {
                            _child_idx--;
                            return *this;
                        }
                        _on_children = false;
                        return *this;
                    }

                    if (ret->_parent) {
                        _ptr = ret->_parent;
                        _on_children = true;
                        _child_idx = 0;

                        for (auto *it : ret->_parent->_children) {
                            if (it == ret) break;
                            _child_idx++;
                        }

                        if (_child_idx <= 0) {
                            // end() ?
                            _ptr = ret;
                            _child_idx = idx;
                            _on_children = false;
                        } else
                            _child_idx--;
                    } else {
                        _ptr = nullptr;
                    }

                    return (*this);
                }

                pointer _ptr{};
                bool _on_children{};
                size_type _child_idx{};
            };

            using reverse_iterator = rev_preorder_iter_data;
            using const_reverse_iterator = const reverse_iterator;
            reverse_iterator rbegin() { return reverse_iterator::begin(this); }
            const_reverse_iterator rbegin() const { return reverse_iterator::begin(this); }
            reverse_iterator rend() { return const_reverse_iterator::end(this); }
            const_reverse_iterator rend() const { return const_reverse_iterator::end(this); }

            struct children_first_iter_data {

                // iterator traits
                using difference_type = std::ptrdiff_t;
                using value_type = Node;
                using pointer = value_type *;
                using reference = value_type &;
                using iterator_category = std::bidirectional_iterator_tag;

                children_first_iter_data() {}
                children_first_iter_data(pointer ptr_, bool on_children_ = false, size_type idx_ = 0)
                    : _ptr(ptr_)
                    , _on_children(on_children_)
                    , _child_idx(idx_) {}
                children_first_iter_data(const children_first_iter_data &o)
                    : _ptr(o._ptr)
                    , _on_children(o._on_children)
                    , _child_idx(o._child_idx) {}
                children_first_iter_data &operator=(const children_first_iter_data &o) {
                    _ptr = o._ptr, _on_children = o._on_children, _child_idx = o._child_idx;
                    return *this;
                }

                bool operator==(children_first_iter_data const &r) const { return _ptr == r._ptr && _on_children == r._on_children && _child_idx == r._child_idx; }
                bool operator!=(children_first_iter_data const &r) const { return _ptr != r._ptr || _on_children != r._on_children || _child_idx != r._child_idx; }
                reference operator*() const {
                    auto &ret = _ptr;
                    if (_on_children) {
                        if (ret->_children.empty() || _child_idx >= ret->_children.size())
                            return *ret;
                        return *(ret->_children[_child_idx]);
                    }
                    return *ret;
                }
                children_first_iter_data &operator++() { return _incr(); }
                children_first_iter_data operator++(int) {
                    children_first_iter_data copy{_ptr, _on_children, _child_idx};
                    ++(*this);
                    return copy;
                }
                static children_first_iter_data begin(pointer root_) {
                    if (root_) {
                        pointer p = root_, last{nullptr};
                        while (p) {
                            last = p;
                            if (p->_children.empty())
                                break;
                            p = (p->_children[0]);
                        }
                        return children_first_iter_data{last, !last->_children.empty()};
                    }
                    return children_first_iter_data{root_};
                }
                static children_first_iter_data end(pointer root_) {
                    // NodePtr p = root_, last{nullptr};
                    // while (p) {
                    //     last = p;
                    //     if (p->_children.empty())
                    //         break;
                    //     p = (p->_children[p->_children.size() - 1]);
                    // }
                    // return children_first_iter_data{last, !last->_children.empty()};
                    if (root_)
                        return children_first_iter_data{root_, false, root_->_children.size()};
                    return children_first_iter_data{root_};
                }

            private:
                children_first_iter_data &_incr() {
                    auto ret = _ptr;
                    auto idx = _child_idx;

                    if (_on_children) {
                        if (!(ret->_children.empty() || _child_idx >= ret->_children.size())) {
                            _child_idx++;
                            return *this;
                        }
                        _on_children = false;
                        return *this;
                    }

                    if (ret->_parent) {
                        _ptr = ret->_parent;
                        _on_children = true;
                        _child_idx = 0;

                        for (auto *it : ret->_parent->_children) {
                            _child_idx++;
                            if (it == ret) break;
                        }

                        if (_child_idx >= ret->_parent->_children.size()) {
                            // end() ?
                            _ptr = ret;
                            _child_idx = idx;
                            _on_children = false;
                        }
                    }

                    return (*this);
                }

                pointer _ptr{};
                bool _on_children{};
                size_type _child_idx{};
            };
        }; // struct generic_node_t

    } // namespace detail

    template<typename Data, typename Node = detail::generic_node_t<Data>>
    class tree_t : detail::generic_tree_ops<Node> {
    public:
        using Self = tree_t<Data, Node>;
        using BaseT = detail::generic_tree_ops<Node>;
        using NodeT = Node;
        using NodePtr = Node *;
        using iterator = typename Node::iterator;
        using const_iterator = typename Node::const_iterator;
        using reverse_iterator = typename Node::reverse_iterator;
        using const_reverse_iterator = typename Node::const_reverse_iterator;

        ~tree_t() { clear(); }

        void clear() override {
            if (_root) delete _root;
            BaseT::clear();
        }

        void insert(Data const &data) {
            if (!_root) {
                _root = new NodeT{data};
                return;
            }
            _root->insert(data);
        }
        void insert(Data &&data) {
            if (!_root) {
                _root = new NodeT{data};
                return;
            }
            _root->insert(std::move(data));
        }
        template<typename... Args>
        void emplace(Args &&...args) {
            if (!_root) {
                _root = new NodeT{std::forward<Args>(args)...};
                return;
            }
            _root->emplace(std::forward<Args>(args)...);
        }

        const NodePtr root() const { return _root; }
        NodePtr root() { return _root; }

        iterator begin() { return _root->begin(); }
        iterator end() { return _root->end(); }
        const_iterator begin() const { return _root->begin(); }
        const_iterator end() const { return _root->end(); }
        reverse_iterator rbegin() { return _root->rbegin(); }
        reverse_iterator rend() { return _root->rend(); }
        const_reverse_iterator rbegin() const { return _root->rbegin(); }
        const_reverse_iterator rend() const { return _root->rend(); }

        // void find();

    private:
        NodePtr _root{nullptr};
    }; // class tree_t

} // namespace dp::tree


#endif //DESIGN_PATTERNS_CXX_DP_TREE_HH
