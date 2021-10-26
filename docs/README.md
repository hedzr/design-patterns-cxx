# design-patterns-in-cxx*(17)*

![CMake Build Matrix](https://github.com/hedzr/design-patterns-cxx/workflows/CMake%20Build%20Matrix/badge.svg) <!-- 
![CMake Build Matrix](https://github.com/hedzr/design-patterns-cxx/workflows/CMake%20Build%20Matrix/badge.svg?event=release) 
--> [![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/hedzr/design-patterns-cxx.svg?label=release)](https://github.com/hedzr/design-patterns-cxx/releases)

`design-patterns-cxx` is an exercise about the reusable templates of design patterns, in C++17.


## Build Options

### Build with CMake

> 1. gcc 10+: passed
> 2. clang 12+: passed
> 3. msvc build tool 16.7.2, 16.8.5 (VS2019 or Build Tool) passed

ninja is optional for faster building.

```bash
# configure
cmake -S . -B build/ -G Ninja
# build
cmake --build build/
# install
cmake --install build/
# Or:cmake --build build/ --target install
#
# Sometimes sudo it:
#   sudo cmake --build build/ --target install
# Or manually:
#   cmake --install build/ --prefix ./install --strip
#   sudo cp -R ./install/include/* /usr/local/include/
#   sudo cp -R ./install/lib/cmake/undo_cxx /usr/local/lib/cmake/
```

## LICENSE

MIT
