// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/18.
//

#include "design_patterns/dp-debug.hh" // type_name<T>

#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <string_view>

///////////////////////////////////////////////////////////////////

class test;

int main() {
    using std::cout;
    using std::endl;
    using namespace dp::debug;
    
    cout << "test                     : " << type_name<test>() << endl;

    cout << "const int*&              : " << type_name<const int *&>() << endl;
    cout << "unsigned int             : " << type_name<unsigned int>() << endl;

    const int ic = 42;
    const int *pic = &ic;
    const int *&rpic = pic;
    cout << "const int                : " << type_name<decltype(ic)>() << endl;
    cout << "const int*               : " << type_name<decltype(pic)>() << endl;
    cout << "const int*&              : " << type_name<decltype(rpic)>() << endl;

    cout << "void                     : " << type_name<void>() << endl;

    cout << "std::string              : " << type_name<std::string>() << endl;
    cout << "std::vector<std::string> : " << type_name<std::vector<std::string>>() << endl;
}