# FreeBSD

Status: not working

While the project libraries appear to build correctly, they do not load correctly.

It seems the line
```
Client *instance = dynamic_cast<Client *>(this->plugins["client_" + name].newInstance());
```
fails. If it is replaced with a C-style cast, it works. So it's something RTTI related.
Getting the typeid() releals a PluginLoadable object, the class all loadable module classes
inherit from.

Interestingly, other dynamic_cast<>s do work. Note that this is the client library, the first
library loaded by the core. The Client class will load other modules and those can do a 
successful dynamic_cast<>.

# Dependencies

FreeBSD comes with clang installed by default.
Please note FreeBSD comes with the BSD variant of Make. This project uses the GNU variant of Make, therefore `gmake` needs to be used on FreeBSD.
To resolve dependencies, `pkgconf` is used. `bash` and `vim` are nice to have around. 


```
pkg install git gmake pkgconf bash vim
```


Libraries used by the project 
```
pkg install nlohmann-json libretls icu cxxopts
```

# Building

Note to use `gmake` in stead of make. The default compiler on FreeBSD is clang. This needs to be specified. 
Go to the module you want to build and run

```
gmake COMPILER=clang
```

# Running

Go to the output directory, eg. `out/clang/freebsd/amd64/debug` and run
```
LD_LIBRARY_PATH=lib bin/blaatbot2025 -c config_file.json
```

