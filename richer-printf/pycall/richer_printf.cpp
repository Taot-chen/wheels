#include <iostream>
#include "richer_printf.h"

extern "C" {
    void logging(char* msg, char* color) {
        switch (hash(color)) {    //  编译器计算 hash 值
            case hash("red"):
                PRINT_RED("%s", msg);
                break;
            case hash("green"):
                PRINT_GREEN("%s", msg);
                break;
            case hash("yellow"):
                PRINT_YELLOW("%s", msg);
                break;
            case hash("blue"):
                PRINT_BLUE("%s", msg);
                break;
            case hash("magenta"):
                PRINT_MAGENTA("%s", msg);
                break;
            case hash("cyan"):
                PRINT_CYAN("%s", msg);
                break;
            case hash("white"):
                PRINT_WHITE("%s", msg);
                break;
            default:
                std::cout << "Unsupport color " << color << std::endl;
        }
    }

    void help() {
        std::cout << "Color Options:" << std::endl
                  << "\t- red" << std::endl
                  << "\t- green" << std::endl
                  << "\t- yellow" << std::endl
                  << "\t- blue" << std::endl
                  << "\t- magenta" << std::endl
                  << "\t- cyan" << std::endl
                  << "\t- white" << std::endl;
    }
}
