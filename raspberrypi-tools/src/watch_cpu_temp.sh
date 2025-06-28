awk '{printf "CPU温度：%.2f°C\n", $1/1000}' /sys/class/thermal/thermal_zone0/temp
