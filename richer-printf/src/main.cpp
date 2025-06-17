#include <iostream>
#include "richer_printf.h"

void test_richer_printf(char *msg) {
    PRINT("RED:");
    PRINT_RED("%s", msg);
    PRINT("GREEN:");
    PRINT_GREEN("%s", msg);
    PRINT("YELLOW:");
    PRINT_YELLOW("%s", msg);
}

int main() {
    char msg[] = "test richer printf\n";
    test_richer_printf(msg);
    return 0;
}
