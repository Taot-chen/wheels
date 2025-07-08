#!/bin/bash
INTERFACES=$(ls /sys/class/net | grep -Ev 'lo|docker|veth|br-')

# 使用 awk 替代 bc
format_speed() {
    local speed=$1
    if (( speed >= 1073741824 )); then
        awk "BEGIN {printf \"%.2f GB/s\", $speed / (1024 * 1024 * 1024)}"
    elif (( speed >= 1048576 )); then
        awk "BEGIN {printf \"%.2f MB/s\", $speed / (1024 * 1024)}"
    elif (( speed >= 1024 )); then
        echo "$((speed / 1024)) KB/s"
    else
        echo "$((speed)) B/s"
    fi
}

declare -A RX_PREV TX_PREV
for intf in $INTERFACES; do
    RX_PREV[$intf]=$(cat /sys/class/net/$intf/statistics/rx_bytes 2>/dev/null || echo 0)
    TX_PREV[$intf]=$(cat /sys/class/net/$intf/statistics/tx_bytes 2>/dev/null || echo 0)
done

# 清屏并打印表头
clear
printf "\e[1;36m%-10s %-20s %-20s\e[0m\n" "网卡" "下载 ▼" "上传 ▲"
echo "--------------------------------------"

while true; do
    printf "\e[3H"  
    OUTPUT=""
    for intf in $INTERFACES; do
        RX_NOW=$(cat /sys/class/net/$intf/statistics/rx_bytes 2>/dev/null || echo 0)
        TX_NOW=$(cat /sys/class/net/$intf/statistics/tx_bytes 2>/dev/null || echo 0)
        RX_SPEED=$((RX_NOW - ${RX_PREV[$intf]}))
        TX_SPEED=$((TX_NOW - ${TX_PREV[$intf]}))
        RX_PREV[$intf]=$RX_NOW
        TX_PREV[$intf]=$TX_NOW
        
        # 每行输出一个网卡（固定宽度对齐）
        printf "%-10s \e[32m%-20s\e[0m \e[31m%-20s\e[0m\n" \
            "$intf" \
            "$(format_speed $RX_SPEED)" \
            "$(format_speed $TX_SPEED)"
    done
    sleep 1
done
