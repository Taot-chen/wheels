树莓派日常使用中的一些实用工具：

* [clash_proxy](https://github.com/Taot-chen/wheels/blob/main/raspberrypi-tools/src/clash_proxy.sh): 一键设置代理


* [watch_cpu_temp](https://github.com/Taot-chen/wheels/blob/main/raspberrypi-tools/src/watch_cpu_temp.sh): 查看 CPU 温度


```bash
chmod +x watch_cpu_temp.sh
./watch_cpu_temp.sh
```

![Alt text](./images/image.png)


实时查看：

```bash
watch -n 1 ./watch_cpu_temp.sh
```

![Alt text](./images/image-1.png)
