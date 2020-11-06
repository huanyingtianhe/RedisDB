# RedisDB
Implement Redis using c++.

### Features

1. Base data structure: skiplist, hash table, ziplist, quicklist, intset, Rax
2. Change IO model to c++ version netty or actor model
3. Provide multithread ability: lock manager or mvcc
4. Distributed system feature(raft master elaction, consistent hash)
6. Provide sql interface to access data

### How to build in visual studio code

1. install vcpkg following the steps in https://github.com/microsoft/vcpkg
2. run integrate command of vcpkg
   >vcpkg integrate install
   
   see details in https://github.com/microsoft/vcpkg/blob/master/docs/users/integration.md
3. install spdlog, gtest with vcpkg
4. install CMake Tools and CMake plugin in visual studio code 
5. change CMAKE_TOOLCHAIN_FILE conf in settings.json to your vcpkg path
6. ctrl/cmd + shift + p, select "cmake: build" option, you will see the binary in a few seconds.

**Note**: pay attention to Triplet selection when you install package with vcpkg, it must be consistent with the CMake configure of the project. for example, if the Triplet selection is x86-windows, you should configure the project with x86.