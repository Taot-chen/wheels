# wheels

## Richer-`printf`

[Richer-printf](https://github.com/Taot-chen/wheels/tree/main/richer-printf)


基于`printf`函数，使用宏封装的一套更丰富多彩的日志打印函数：

* 分级打印，并且根据打印等级设置相应的打印颜色。
* 打印所在的文件、行号、函数
* 打印当前时间，支持精确到毫秒


C/C++ 调用示例：[main.cpp](https://github.com/Taot-chen/wheels/blob/main/richer-printf/src/main.cpp)


python 调用使用的是 ctypes 的方法，这种方法可以很方便的直接使用 C++ 的源码，不需要对 C++ 源码做修改。需要先将 C++ 源码编译成共享库，之后再在 python 里面调用。
python 调用示例：[pycall](https://github.com/Taot-chen/wheels/blob/main/richer-printf/pycall)

```bash
cd pycall
g++ -shared -o richer_printf.so -fPIC richer_printf.cpp
```
