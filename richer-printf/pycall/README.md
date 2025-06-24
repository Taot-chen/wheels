一个羽量级的 python 日志系统

---------

python 调用使用的是 ctypes 的方法，这种方法可以很方便的直接使用 C++ 的源码，不需要对 C++ 源码做修改。需要先将 C++ 源码编译成共享库，之后再在 python 里面调用。

```bash
cd pycall
g++ -shared -o richer_printf.so -fPIC richer_printf.cpp
```
