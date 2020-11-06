# RedisDB
Implement Redis using c++.

### Feature

1. Base data structure: skiplist, hash table, ziplist, quicklist, intset, Rax

2. Change IO model to c++ version netty or actor model

3. Provide multithread ability: lock manager or mvcc

4. Cluster feature(raft master elaction, consistent hash)

6. Provide sql interface to access data

### How to build

1. install vcpkg following the steps in https://github.com/microsoft/vcpkg

2. install spdlog, gtest with vcpkg

3. change CMAKE_TOOLCHAIN_FILE in settings.json to your vcpkg path
