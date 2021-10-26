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

namespace dp::tree {

    template<typename T>
    class node_t {
    public:
    };

    class rb_tree {
    public:
        rb_tree() {}
        ~rb_tree() { clear(); }
        
        struct rb_node_t {
            bool color_is_red{};
            int key{};
            rb_node_t *left{};
            rb_node_t *right{};
            rb_node_t *parent{};

            // rb_node_t() {}
            ~rb_node_t() {
                if (left) delete left;
                if (right) delete right;
            }
        };
        using rb_tree_t = rb_node_t *;
        
    private:
        rb_node_t *_root{nullptr};

    public:
        void clear() {
            if (_root) {
                delete _root;
                _root = nullptr;
            }
        }

        void insert(int v) {
            if (_root == nullptr)
                _root = new rb_node_t{false};
            auto *node = new rb_node_t{false, v};
            insert_rbt(_root, node);
        }

    private:
        int insert_rbt(rb_node_t *&root, rb_node_t *node);
        int insert_rbt_fixup(rb_node_t *&root, rb_node_t *node);
        static rb_node_t *rbt_rotate_right(rb_node_t *&root, rb_node_t *node);
        static rb_node_t *rbt_rotate_left(rb_node_t *&root, rb_node_t *node);

    public:
        bool erase(int v) {
            if (_root)
                return delete_rbt(_root, v) == 0;
            return false;
        }

    private:
        int delete_rbt(rb_node_t *&root, int key);
        static int delete_rbt_fixup(rb_node_t *&root, rb_node_t *node, rb_node_t *parent);
    };

} // namespace dp::tree

namespace dp::tree {

    inline int rb_tree::insert_rbt(rb_node_t *&root, rb_node_t *node) {
        rb_node_t *p = root;
        rb_node_t *last = nullptr;

        //find the position we need to insert
        while (p) {
            last = p;
            if (p->key == node->key)
                return 1;
            if (p->key > node->key)
                p = p->left;
            else
                p = p->right;
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

    inline int rb_tree::insert_rbt_fixup(rb_node_t *&root, rb_node_t *node) {
        rb_node_t *parent;
        rb_node_t *grandpa;

        //If parent exist, and the color of parent is RED
        while ((parent = node->parent) && parent->color_is_red) {
            grandpa = parent->parent;

            // parent node is grandpa node's left child (grandpa shouldn't be NULL since parent->color_is_red)
            if (grandpa->left == parent) {
                rb_node_t *uncle = grandpa->right;

                //Case 1: uncle is RED
                if (uncle && uncle->color_is_red) {
                    parent->color_is_red = false;
                    uncle->color_is_red = false;
                    grandpa->color_is_red = true;

                    node = grandpa;
                    continue;
                }

                //Case 2: uncle is BLACK, and node is parent's right child
                if (parent->right == node) {
                    rbt_rotate_left(root, parent);

                    // reset parent and node pointer
                    rb_node_t *tmp;
                    tmp = parent, parent = node, node = tmp;

                    //Here successful convert Case 2 to Case3
                }

                //Case 3: uncle is BLACK, and node is parent's left child
                parent->color_is_red = false;
                grandpa->color_is_red = true;
                rbt_rotate_right(root, grandpa);

            } else {
                rb_node_t *uncle = grandpa->left;

                //Case 1: uncle is RED
                if (uncle && uncle->color_is_red) {
                    parent->color_is_red = false;
                    uncle->color_is_red = false;
                    grandpa->color_is_red = true;

                    node = grandpa;
                    continue;
                }

                //Case 2: uncle is BLACK, and node is parent's left child
                if (parent->left == node) {
                    rbt_rotate_right(root, parent);

                    //reset parent and node pointer
                    rb_node_t *tmp;
                    tmp = parent, parent = node, node = tmp;

                    //Here success convert Case 2 to Case 3
                }

                //Case 3: uncle is BLACK, and node is parent's right child
                parent->color_is_red = false;
                grandpa->color_is_red = true;
                rbt_rotate_left(root, grandpa);
            }
        }

        root->color_is_red = false;
        return 0;
    }

    inline rb_tree::rb_node_t *rb_tree::rbt_rotate_right(rb_node_t *&root, rb_node_t *node) {
        rb_node_t *left = node->left;

        if (node->left == left->right) {
            left->right->parent = node;
        }

        left->right = node;
        node->left = nullptr;

        if (left->parent == node->parent) {
            if (node->parent->left == node) {
                node->parent->left = left;
            } else {
                node->parent->right = left;
            }
        } else {
            root = left;
        }

        node->parent = left;
        return left;
    }

    inline rb_tree::rb_node_t *rb_tree::rbt_rotate_left(rb_node_t *&root, rb_node_t *node) {
        rb_node_t *right = node->right;

        if (node->right == right->left) {
            right->left->parent = node;
        }

        right->left = node;
        node->right = nullptr;

        if (right->parent == node->parent) {
            if (node->parent->left == node) {
                node->parent->left = right;
            } else {
                node->parent->right = right;
            }
        } else {
            root = right;
        }

        node->parent = right;
        return right;
    }

    inline int rb_tree::delete_rbt(rb_node_t *&root, int key) {
        rb_node_t *p = root;

        //find the node
        while (p) {
            if (p->key == key)
                break;
            if (p->key > key)
                p = p->left;
            else
                p = p->right;
        }

        if (!p)
            return -1;

        if (p->left && p->right) {
            //get successor node
            rb_node_t *successor = p->right;

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

            rb_node_t *successor_child = successor->right;
            rb_node_t *successor_parent = successor->parent;
            bool color_is_red = successor->color_is_red; //save the color

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

        rb_node_t *child = nullptr;
        rb_node_t *parent = nullptr;
        bool color_is_red;

        if (p->left)
            child = p->left;
        else
            child = p->right;

        parent = p->parent;
        color_is_red = p->color_is_red; //save the color

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

        p->left = nullptr;
        p->right = nullptr;
        delete (p);
        return 0;
    }

    inline int rb_tree::delete_rbt_fixup(rb_node_t *&root, rb_node_t *node, rb_node_t *parent) {
        rb_node_t *brother = nullptr;

        while ((!node || !node->color_is_red) && node != root) {
            if (parent->left == node) {
                //The left branch

                //Note: brother can't be NULL, because we have delete a black node,
                //and the tree is balanced before we delete the node
                brother = parent->right;

                if (brother->color_is_red) {
                    //1) Case 1: x's brother is COLOR_RED
                    brother->color_is_red = false;
                    parent->color_is_red = true;
                    rbt_rotate_left(root, node);
                    brother = parent->right;
                }

                if ((!brother->left || !brother->left->color_is_red) &&
                    (!brother->right || !brother->right->color_is_red)) {
                    //2) Case 2: x's brother is COLOR_BLACK, is its two child is NULL or COLOR_BLACK
                    brother->color_is_red = true;
                    node = parent;
                    parent = node->parent;
                } else {
                    if (!brother->right || !brother->right->color_is_red) {
                        //3) Case 3: x's brother is COLOR_BLACK, and brother left child is COLOR_RED,
                        // right child is COLOR_BLACK
                        brother->left->color_is_red = false;
                        brother->color_is_red = true;
                        rbt_rotate_right(root, brother);
                        brother = parent->right;
                    }

                    //4) Case 4: x's brother is BLACK, and brother's right child is RED
                    brother->color_is_red = parent->color_is_red;
                    parent->color_is_red = false;
                    brother->right->color_is_red = false;
                    rbt_rotate_left(root, parent);

                    node = root;
                    break;
                }

            } else {
                //The right branch
                brother = parent->left;

                if (brother->color_is_red) {
                    //1) Case 1: x's brother is RED
                    brother->color_is_red = false;
                    parent->color_is_red = true;
                    rbt_rotate_right(root, parent);
                    brother = parent->left;
                }

                if ((!brother->left || !brother->left->color_is_red) &&
                    (!brother->right || !brother->right->color_is_red)) {
                    //2) Case 2: x's brother is BLACK, is its two child is NULL or BLACK
                    brother->color_is_red = true;
                    node = parent;
                    parent = node->parent;
                } else {
                    if (!brother->left || !brother->left->color_is_red) {
                        //3) Case 3: x's brother is LACK, and brother right child is RED,
                        // left child is BLACK
                        brother->right->color_is_red = false;
                        brother->color_is_red = true;
                        rbt_rotate_left(root, brother);
                        brother = parent->left;
                    }

                    //4) Case 4: x's brother is BLACK, and brother's left child is RED
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


#endif //DESIGN_PATTERNS_CXX_DP_TREE_HH
