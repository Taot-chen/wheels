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


## keyboard-stats

[keyboard-stats](https://github.com/Taot-chen/wheels/tree/main/keyboard-stats)

一个对键盘设备进行监控统计的工具：

* 按键统计：记录每个键的按下次数
* 打字速度：计算每分钟输入字符数
* 敲键速度：计算每秒按键次数
* 按键间隔：统计两次按键之间的时间间隔
* 结果保存：将统计结果保存到文本文件



----------

**TODO**

- [ ] 添加图形界面或实时数据显示
- [ ] 更多的统计信息
- [ ] 多设备支持
- [ ] 更全面的键码映射
- [ ] 资源占用优化


## raspberrypi-tools

[raspberrypi-tools](https://github.com/Taot-chen/wheels/tree/main/raspberrypi-tools)

树莓派日常使用中的一些实用工具：

* [clash_proxy](https://github.com/Taot-chen/wheels/blob/main/raspberrypi-tools/src/clash_proxy.sh): 一键设置代理
* [watch_cpu_temp](https://github.com/Taot-chen/wheels/blob/main/raspberrypi-tools/src/watch_cpu_temp.sh): 查看 CPU 温度



## watch-net

[watch-net](https://github.com/Taot-chen/wheels/tree/main/watch-net)

一个实用的 linux 网卡监控脚本：

```bash
./watch_net.sh
# 运行脚本（输出示例）
网卡     下载 ▼           上传 ▲
--------------------------------------
eth0       1 KB/s               2 KB/s
wlan0      232 B/s              0 B/s
```
