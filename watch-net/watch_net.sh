#!/bin/bash
# 自动获取所有非回环网卡名称
INTERFACES=$(ls /sys/class/net | grep -v 'lo')
LOG="/var/log/net_speed.log"  # 可选日志路径
THRESHOLD=10240  # 流量阈值（KB/s）

# 单位转换函数（B → KB/MB/GB）
format_speed() {
    local speed=$1
    if (( speed >= 1024**2 )); then
        printf "%.2f GB/s" $(echo "$speed / 1024^2" | bc -l)
    elif (( speed >= 1024 )); then
        printf "%.2f MB/s" $(echo "$speed / 1024" | bc -l)
    else
        printf "%d KB/s" $speed
    fi
}

# 初始化流量计数器
declare -A RX_PREV TX_PREV
for intf in $INTERFACES; do
    RX_PREV[$intf]=$(cat /sys/class/net/$intf/statistics/rx_bytes)
    TX_PREV[$intf]=$(cat /sys/class/net/$intf/statistics/tx_bytes)
done

# 主循环：动态刷新多网卡流量
while true; do
    sleep 1
    OUTPUT=""  # 清空输出缓存

    for intf in $INTERFACES; do
        # 获取当前流量
        RX_NOW=$(cat /sys/class/net/$intf/statistics/rx_bytes)
        TX_NOW=$(cat /sys/class/net/$intf/statistics/tx_bytes)
        
        # 计算1秒内流量差
        RX_SPEED=$(( RX_NOW - RX_PREV[$intf] ))
        TX_SPEED=$(( TX_NOW - TX_PREV[$intf] ))
        RX_PREV[$intf]=$RX_NOW
        TX_PREV[$intf]=$TX_NOW
        
        # 格式化输出
        OUTPUT+="[$intf] ▼ \e[32m$(format_speed $RX_SPEED)\e[0m ▲ \e[31m$(format_speed $TX_SPEED)\e[0m | "
        
        # 超阈值告警（可选）
        if (( RX_SPEED > THRESHOLD || TX_SPEED > THRESHOLD )); then
            echo "[$(date)] 网卡 $intf 流量异常 ▼ ${RX_SPEED}KB/s ▲ ${TX_SPEED}KB/s" >> "$LOG"
        fi
    done

    # 单行刷新显示（ANSI转义码）
    printf "\r\e[2K%s" "${OUTPUT%| }"  # 移除末尾竖线分隔符
done
