## Linux 下网速监控

### 1 核心特性 ⚡

* 自动获取网卡名​ 🤖

通过 `/sys/class/net` 目录获取所有网卡名，排除回环接口 `lo`:

```bash
INTERFACES=$(ls /sys/class/net | grep -v 'lo')  # 过滤掉lo网卡
```

兼容物理网卡、虚拟网卡（如 `eth0`, `wlan0`, `docker0`），无需手动指定。




* ​​多网卡并行监控​ 🕙

使用 `printf "\r\e[2K"` 清空当前行并回退到行首，实时更新所有网卡流量

▼ 下载绿色（`\e[32m）`，▲ 上传红色`（\e[31m）`，网卡名用 `[]` 标注区分



* 性能与健壮性 🚀

`declare -A RX_PREV TX_PREV` 为每个网卡独立存储计数器，避免多网卡数据覆盖

直接读取 `/sys/class/net/<网卡>/statistics/{rx,tx}_bytes`，效率高于 `ifconfig`



* 拓展功能

流量超过 THRESHOLD 时自动记录到日志文件（取消 `LOG` 变量注释即可启用）

自动根据流量大小切换单位（KB/MB/GB），避免数值过大




### 2 使用 ⏭️

```bash
chmod +x ./watch_net.sh
./watch_net.sh
# 运行脚本（输出示例）
网卡     下载 ▼           上传 ▲
--------------------------------------
eth0       1 KB/s               2 KB/s
wlan0      232 B/s              0 B/s
```

若需监控指定网卡（如只监控 `eth*`），修改：

```bash
INTERFACES=$(ls /sys/class/net | grep '^eth')
```

调整 `sleep 1` 为 `sleep 2` 可减少 CPU 占用（适合嵌入式设备）.



