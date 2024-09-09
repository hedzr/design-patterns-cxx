// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/20.
//

#include "design_patterns_cxx/dp-tree.hh"

#include "design_patterns_cxx/dp-common.hh"
#include "design_patterns_cxx/dp-def.hh"

#include "design_patterns_cxx/dp-log.hh"
#include "design_patterns_cxx/dp-util.hh"
#include "design_patterns_cxx/dp-x-test.hh"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>


template<typename Data>
void tree_info(dp::tree::rb_tree<Data> &t) {
  std::cout << "-----------------------------------------------" << '\n';
  std::cout << "  tree count: " << t.count() << '\n';
  std::cout << "  tree height: " << t.height() << '\n';
  std::cout << "  in-order: ";
  t.traverse_in_order(t.root(), [](auto *node) {
    std::cout << node->key << ", ";
  });
  std::cout << '\n';
  std::cout << "  level-order: ";
  t.traverse_level_order(t.root(), [](auto *node) {
    std::cout << node->key << ", ";
  });
  std::cout << '\n';
}

void test_rb_tree_decr() {
  dp::tree::rb_tree<int> t;
  UNUSED(t);

  for (auto v : {7, 6, 5, 4, 3, 2, 1}) {
    t.insert(v);
    tree_info(t);
  }

  t.erase(4);
  tree_info(t);
  assertm(t.count() == 6, "bad tree count");
}

void test_rb_tree_incr() {
  dp::tree::rb_tree<int> t;
  UNUSED(t);

  for (auto v : {1, 2, 3, 4, 5, 6, 7}) {
    t.insert(v);
    tree_info(t);
  }

  t.erase(4);
  tree_info(t);
  assertm(t.count() == 6, "bad tree count");
}

class tree_data {
public:
  int val;
  std::string str;

  friend bool operator==(tree_data const &lhs, int const &rhs) {
    return lhs.val == rhs;
  }
  friend bool operator==(tree_data const &lhs, tree_data const &rhs) {
    return lhs.val == rhs.val;
  }
  friend bool operator<(tree_data const &lhs, tree_data const &rhs) {
    return lhs.val < rhs.val;
  }
  friend std::ostream &operator<<(std::ostream &os, tree_data const &o) {
    return os << '(' << o.val << ',' << o.str << ')';
  }
};

void test_rb_tree() {
  dp::tree::rb_tree<tree_data> t;
  UNUSED(t);

  std::array<char, 128> buf;
  for (auto v : {1, 2, 3, 4, 5, 6, 7}) {
    std::snprintf(buf.data(), buf.size(), "str#%d", v);
    t.emplace(v, buf.data());
    tree_info(t);
  }
}

void test_invalid_iterator() {
  std::vector<int> vi{3, 7};
  auto it = vi.begin();
  it = vi.insert(it, 11);
  vi.insert(it, 5000, 23);
  vi.insert(it, 1, 31); // should crash here
  std::cout << (*it) << '\n';
  return;
}

namespace customized_iterators {
#if 0
    template <class T, class A = std::allocator<T> >
    class X {
    public:
        typedef A allocator_type;
        typedef typename A::value_type value_type; 
        typedef typename A::reference reference;
        typedef typename A::const_reference const_reference;
        typedef typename A::difference_type difference_type;
        typedef typename A::size_type size_type;
        
        class iterator { 
        public:
            typedef typename A::difference_type difference_type;
            typedef typename A::value_type value_type;
            typedef typename A::reference reference;
            typedef typename A::pointer pointer;
            typedef std::random_access_iterator_tag iterator_category; //or another tag

            iterator();
            iterator(const iterator&);
            ~iterator();

            iterator& operator=(const iterator&);
            bool operator==(const iterator&) const;
            bool operator!=(const iterator&) const;
            bool operator<(const iterator&) const; //optional
            bool operator>(const iterator&) const; //optional
            bool operator<=(const iterator&) const; //optional
            bool operator>=(const iterator&) const; //optional

            iterator& operator++();
            iterator operator++(int); //optional
            iterator& operator--(); //optional
            iterator operator--(int); //optional
            iterator& operator+=(size_type); //optional
            iterator operator+(size_type) const; //optional
            friend iterator operator+(size_type, const iterator&); //optional
            iterator& operator-=(size_type); //optional            
            iterator operator-(size_type) const; //optional
            difference_type operator-(iterator) const; //optional

            reference operator*() const;
            pointer operator->() const;
            reference operator[](size_type) const; //optional
        };
        class const_iterator {
        public:
            typedef typename A::difference_type difference_type;
            typedef typename A::value_type value_type;
            typedef typename const A::reference reference;
            typedef typename const A::pointer pointer;
            typedef std::random_access_iterator_tag iterator_category; //or another tag

            const_iterator ();
            const_iterator (const const_iterator&);
            const_iterator (const iterator&);
            ~const_iterator();

            const_iterator& operator=(const const_iterator&);
            bool operator==(const const_iterator&) const;
            bool operator!=(const const_iterator&) const;
            bool operator<(const const_iterator&) const; //optional
            bool operator>(const const_iterator&) const; //optional
            bool operator<=(const const_iterator&) const; //optional
            bool operator>=(const const_iterator&) const; //optional

            const_iterator& operator++();
            const_iterator operator++(int); //optional
            const_iterator& operator--(); //optional
            const_iterator operator--(int); //optional
            const_iterator& operator+=(size_type); //optional
            const_iterator operator+(size_type) const; //optional
            friend const_iterator operator+(size_type, const const_iterator&); //optional
            const_iterator& operator-=(size_type); //optional            
            const_iterator operator-(size_type) const; //optional
            difference_type operator-(const_iterator) const; //optional

            reference operator*() const;
            pointer operator->() const;
            reference operator[](size_type) const; //optional
        };

        typedef std::reverse_iterator<iterator> reverse_iterator; //optional
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator; //optional

        X();
        X(const X&);
        ~X();

        X& operator=(const X&);
        bool operator==(const X&) const;
        bool operator!=(const X&) const;
        bool operator<(const X&) const; //optional
        bool operator>(const X&) const; //optional
        bool operator<=(const X&) const; //optional
        bool operator>=(const X&) const; //optional

        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;
        iterator end();
        const_iterator end() const;
        const_iterator cend() const;
        reverse_iterator rbegin(); //optional
        const_reverse_iterator rbegin() const; //optional
        const_reverse_iterator crbegin() const; //optional
        reverse_iterator rend(); //optional
        const_reverse_iterator rend() const; //optional
        const_reverse_iterator crend() const; //optional

        reference front(); //optional
        const_reference front() const; //optional
        reference back(); //optional
        const_reference back() const; //optional
        template<class ...Args>
        void emplace_front(Args&&...); //optional
        template<class ...Args>
        void emplace_back(Args&&...); //optional
        void push_front(const T&); //optional
        void push_front(T&&); //optional
        void push_back(const T&); //optional
        void push_back(T&&); //optional
        void pop_front(); //optional
        void pop_back(); //optional
        reference operator[](size_type); //optional
        const_reference operator[](size_type) const; //optional
        reference at(size_type); //optional
        const_reference at(size_type) const; //optional

        template<class ...Args>
        iterator emplace(const_iterator, Args&&...); //optional
        iterator insert(const_iterator, const T&); //optional
        iterator insert(const_iterator, T&&); //optional
        iterator insert(const_iterator, size_type, T&); //optional
        template<class iter>
        iterator insert(const_iterator, iter, iter); //optional
        iterator insert(const_iterator, std::initializer_list<T>); //optional
        iterator erase(const_iterator); //optional
        iterator erase(const_iterator, const_iterator); //optional
        void clear(); //optional
        template<class iter>
        void assign(iter, iter); //optional
        void assign(std::initializer_list<T>); //optional
        void assign(size_type, const T&); //optional

        void swap(X&);
        size_type size() const;
        size_type max_size() const;
        bool empty() const;

        A get_allocator() const; //optional
    };
    template <class T, class A = std::allocator<T> >
    void swap(X<T,A>&, X<T,A>&); //optional
#endif

  template<long FROM, long TO>
  class Range {
  public:
    // member typedefs provided through inheriting from std::iterator
    class iterator : public std::iterator<std::forward_iterator_tag, // iterator_category
                                          long,                      // value_type
                                          long,                      // difference_type
                                          const long *,              // pointer
                                          const long &               // reference
                                          > {
      long num = FROM;

    public:
      iterator(long _num = 0)
          : num(_num) {}
      iterator &operator++() {
        num = TO >= FROM ? num + 1 : num - 1;
        return *this;
      }
      iterator operator++(int) {
        iterator ret_val = *this;
        ++(*this);
        return ret_val;
      }
      bool operator==(iterator other) const { return num == other.num; }
      bool operator!=(iterator other) const { return !(*this == other); }
      long operator*() { return num; }
    };
    iterator begin() { return FROM; }
    iterator end() { return TO >= FROM ? TO + 1 : TO - 1; }
  };

  namespace manually {
    template<long FROM, long TO>
    class Range {
    public:
      class iterator {
        long num = FROM;

      public:
        iterator(long _num = 0)
            : num(_num) {}
        iterator &operator++() {
          num = TO >= FROM ? num + 1 : num - 1;
          return *this;
        }
        iterator operator++(int) {
          iterator ret_val = *this;
          ++(*this);
          return ret_val;
        }
        bool operator==(iterator other) const { return num == other.num; }
        bool operator!=(iterator other) const { return !(*this == other); }
        long operator*() { return num; }
        // iterator traits
        using difference_type = long;
        using value_type = long;
        using pointer = const long *;
        using reference = const long &;
        using iterator_category = std::forward_iterator_tag;
      };
      iterator begin() { return FROM; }
      iterator end() { return TO >= FROM ? TO + 1 : TO - 1; }
    };
  } // namespace manually

  void test_range() {
    manually::Range<5, 13> r;
    for (auto v : r) std::cout << v << ',';
    std::cout << '\n';

    {
      std::list<int> l{1, 2, 3};
      auto it = l.begin();
      ++it;
      (*it) = 5;
    }
    {
      std::vector<int> l{1, 2, 3};
      auto it = l.begin();
      ++it;
      (*it) = 5;
    }
  }

  struct incr {
    int val{};
    incr &operator++() {
      val++;
      return *this;
    }
    incr operator++(int) {
      incr ret_val = *this;
      ++(*this);
      return ret_val;
    }
  };
} // namespace customized_iterators

void test_g_tree() {
  dp::tree::tree_t<tree_data> t;
  UNUSED(t);
  assert(t.rbegin() == t.rend());
  assert(t.begin() == t.end());

  std::array<char, 128> buf;

  //     1
  // 2 3 4 5 6 7
  for (auto v : {1, 2, 3, 4, 5, 6, 7}) {
    std::snprintf(buf.data(), buf.size(), "str#%d", v);
    // t.insert(tree_data{v, buf.data()});
    tree_data vd{v, buf.data()};
    t.insert(std::move(vd));
    // tree_info(t);
  }

  {
    auto v = 8;
    std::snprintf(buf.data(), buf.size(), "str#%d", v);
    tree_data td{v, buf.data()};
    t.insert(td);

    v = 9;
    std::snprintf(buf.data(), buf.size(), "str#%d", v);
    t.emplace(v, buf.data());

    {
      auto b = t.root().begin(), e = t.root().end();
      auto &bNode = (*b), &eNode = (*e);
      std::cout << "::: " << (*bNode) << '\n'; // print bNode.data()
      std::cout << "::: " << (eNode.data()) << '\n';
    }

    {
      int i;
      i = 0;
      for (auto &vv : t) {
        std::cout << i << ": " << (*vv) << ", " << '\n';
        if (i == 8) {
          std::cout << ' ';
        }
        i++;
      }
      std::cout << '\n';
    }

    using T = decltype(t);
    auto it = std::find_if(t.root().begin(), t.root().end(), [](typename T::NodeT &n) -> bool { return (*n) == 9; });

    v = 10;
    std::snprintf(buf.data(), buf.size(), "str#%d", v);
    it->emplace(v, buf.data());

    v = 11;
    std::snprintf(buf.data(), buf.size(), "str#%d", v);
    (*it).emplace(v, buf.data());

#if defined(_DEBUG)
    auto const itv = t.find([](T::const_reference n) { return (*n) == 10; });
    assert(*(*itv) == 10);
#endif
  }

  //

  int i;

  i = 0;
  for (auto &v : t) {
    std::cout << i << ": " << (*v) << ", " << '\n';
    if (i == 8) {
      std::cout << ' ';
    }
    i++;
  }
  std::cout << '\n';

  i = 0;
  for (auto it = t.rbegin(); it != t.rend(); ++it, ++i) {
    auto &v = (*it);
    std::cout << i << ": " << (*v) << ", " << '\n';
    if (i == 8) {
      std::cout << ' ';
    }
  }
  std::cout << '\n';
}

int main() {
  DP_TEST_FOR(test_rb_tree_decr);
  DP_TEST_FOR(test_rb_tree_incr);
  DP_TEST_FOR(test_rb_tree);

  DP_TEST_FOR(test_g_tree);

  DP_TEST_FOR(customized_iterators::test_range);
}