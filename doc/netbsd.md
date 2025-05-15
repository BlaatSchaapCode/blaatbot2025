# NetBSD

I have not been able to compile the project on NetBSD. 
At his point the cause of the issue is my unfamiliarity with NetBSD. 
NetBSD 10.1 comed with gcc 10.5.0 by default, which is too old to compile this project, as it does not support C++23 yet. 


## Attept at gcc 14

I have attempted to install gcc 14.


```
# pkgin install gcc14
```
will install gcc 14 in /usr/pkg/gcc14.  This is not in the path.
Other packages appear in `/usr/pkg/bin`. I have not yet found a documentation how to officially handle this.  I tried to add it in front of the path

```
export PATH=/usr/pkg/gcc14/bin:$PATH
```

While this builds, the linker is complaining about being linked against libstdc++.so.7 while a linked in librarym /usr/pkg/lib/libcuuc.so is linked against libstdc++.so.9. Different C++ library.

Just putting `/usr/pkg/gcc14/bin`  in the path seems not to be the right way.


## Attempt at Clang

I have tried to install clang

```
pkgin install clang
```

This compiler appears in the path right awy, howver, it seems the C++ library is not supporting the latest C++, things like  `<format>` are missing.

