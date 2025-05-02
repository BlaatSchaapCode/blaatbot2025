# Cross Compiling for Windows (mingw)

Note how to cross compile from ArchLinux

## Dependencies

To be built from AUT
* mingw-w64-icu
* mingw-w64-nlohmann-json

Not available anymore in AUR (Will come up with a solution later) 
* mingw-w64-cxxopts


## Cross compiling

The make files in the project allow for cross compiling. By specifying
`TARGET_OS=mingw` as parameter to `make`. Otherwise follow standard 
instructions.

```
[andre@mortar core]$ make TARGET_OS=mingw 
GIT_BRANCH: main
GIT_COMMIT: 6c9989d-dirty
HOST:   linux
TARGET: mingw
find: ‘../../out/gcc/mingw/x86_64/debug/build’: Bestand of map bestaat niet
Compiling     ../../src/clients/Client.cpp...
Compiling     ../../src/network/network.cpp...
Compiling     ../../src/network/Connection.cpp...
Compiling     ../../src/protocol/Protocol.cpp...
Compiling     ../../src/protocol/IRC.cpp...
Compiling     ../../src/utils/compat.cpp...
Compiling     ../../src/utils/logger.cpp...
Compiling     ../../src/utils/splitString.cpp...
Compiling     ../../src/utils/threadName.cpp...
Compiling     ../../src/utils/timer.cpp...
Compiling     ../../src/main.cpp...
Compiling     ../../src/PluginLoader.cpp...
../../src/PluginLoader.cpp: In member function ‘PluginLoader::networkPlugin PluginLoader::loadNetworkPlugin(std::string)’:
../../src/PluginLoader.cpp:84:11: let op: unused variable ‘test’ [-Wunused-variable]
   84 |     int * test =  (int *)GetProcAddress((HMODULE)result.handle, "test");
      |           ^~~~
Linking       ../../out/gcc/mingw/x86_64/debug/bin/blaatbot2025.exe...

```


