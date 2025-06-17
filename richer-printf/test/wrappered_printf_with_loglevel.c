#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// 颜色定义
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

// 分级打印宏封装
#define PRINT_COLOR(color, level, fmt, ...) \
    do { \
        printf("%s[%s] " fmt COLOR_RESET "\n", \
        color, level, ## __VA_ARGS__); \
    } while(0)

// printf 函数封装 with log level
#define PRINT(fmt, ...) PRINT_COLOR(COLOR_WHITE, "INFO", fmt, ## __VA_ARGS__)
#define PRINT_RED(fmt, ...) PRINT_COLOR(COLOR_RED, "ERRO", fmt, ## __VA_ARGS__)
#define PRINT_GREEN(fmt, ...) PRINT_COLOR(COLOR_GREEN, "INFO", fmt, ##__VA_ARGS__)
#define PRINT_YELLOW(fmt, ...) PRINT_COLOR(COLOR_YELLOW, "WARN", fmt, ##__VA_ARGS__)
#define PRINT_BLUE(fmt, ...) PRINT_COLOR(COLOR_BLUE, "INFO", fmt, ##__VA_ARGS__)
#define PRINT_MAGENTA(fmt, ...) PRINT_COLOR(COLOR_MAGENTA, "INFO", fmt, ##__VA_ARGS__)
#define PRINT_CYAN(fmt, ...) PRINT_COLOR(COLOR_CYAN, "INFO", fmt, ##__VA_ARGS__)
#define PRINT_WHITE(fmt, ...) PRINT_COLOR(COLOR_WHITE, "INFO", fmt, ##__VA_ARGS__)


int main() {
    // 常规打印
    PRINT("打印测试");
   
    // 带颜色的打印
    PRINT_RED("红色/RED - 通常用于错误信息");
    PRINT_GREEN("绿色/GREEN - 通常用于成功信息");
    PRINT_YELLOW("黄色/YELLOW - 通常用于警告信息");
    PRINT_BLUE("蓝色/BLUE - 通常用于调试信息");
    PRINT_MAGENTA("洋红色/MAGENTA - 用于特殊标记");
    PRINT_CYAN("青色/CYAN - 用于信息提示");
    PRINT_WHITE("白色/WHITE - 用于普通输出");

    return 0;
}
