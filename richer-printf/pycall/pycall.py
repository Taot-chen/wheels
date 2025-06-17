import ctypes
import os

# 加载 C++ 动态库
lib = ctypes.CDLL(os.path.abspath("./richer_printf.so"))

print("加载成功")

# 设置动态库 logging 函数的返回值和参数类型
lib.logging.restype = ctypes.c_voidp
lib.logging.argtypes = [ctypes.c_char_p, ctypes.c_char_p]

# 调用动态库 help 函数
lib.help()

# 调用动态库 logging 函数
# ctypes.c_char_p(b"Test in Python") --> Python 字符串转成 c_char_p 对象，即 C++ 的字符数组指针
lib.logging(ctypes.c_char_p(b"Test in Python"), ctypes.c_char_p(b"white"))
lib.logging(ctypes.c_char_p(b"Test INFO"), ctypes.c_char_p(b"green"))
lib.logging(ctypes.c_char_p(b"Test WARN"), ctypes.c_char_p(b"yellow"))
lib.logging(ctypes.c_char_p(b"Test ERRO"), ctypes.c_char_p(b"red"))
lib.logging(ctypes.c_char_p(b"Test magenta"), ctypes.c_char_p(b"magenta"))
lib.logging(ctypes.c_char_p(b"Test blue"), ctypes.c_char_p(b"blue"))
lib.logging(ctypes.c_char_p(b"Test cyan"), ctypes.c_char_p(b"cyan"))
lib.logging(ctypes.c_char_p(b"Test Unsupport"), ctypes.c_char_p(b"pink"))
