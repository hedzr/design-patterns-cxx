// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/20.
//

#include "design_patterns/dp-tree.hh"

#include "design_patterns/dp-log.hh"
#include "design_patterns/dp-util.hh"
#include "design_patterns/dp-x-test.hh"

void test_rb_tree() {
    dp::tree::rb_tree t;
    UNUSED(t);
    
    t.insert(1);
    t.insert(2);
    t.insert(3);
    t.insert(4);
    t.insert(5);
    t.insert(6);
    t.insert(7);
    
    t.erase(4);
}

int main() {

    DP_TEST_FOR(test_rb_tree);
}