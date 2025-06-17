#include <stdio.h>

int main () {
    // 常规打印
    printf("常规打印\n");

    // 带颜色的打印
    printf("\033[31m红色/RED - 通常用于错误信息\033[0m\n");
    printf("\033[32m绿色/GREEN - 通常用于成功信息\033[0m\n");
    printf("\033[33m黄色/YELLOW - 通常用于警告信息\033[0m\n");
    printf("\033[34m蓝色/BLUE - 通常用于调试信息\033[0m\n");
    printf("\033[35m洋红色/MAGENTA - 用于特殊标记\033[0m\n");
    printf("\033[36m青色/CYAN - 用于信息提示\033[0m\n");
    printf("\033[37m白色/WHITE - 用于普通输出\033[0m\n");

    return 0;
}
