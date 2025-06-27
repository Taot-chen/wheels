#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <thread>
#include <atomic>
#include <csignal>

using namespace std;
using namespace std::chrono;

// 全局统计数据
map<int, int> key_counts;
steady_clock::time_point program_start;
steady_clock::time_point last_key_time;
vector<double> key_intervals;
atomic<int> total_keys(0);
atomic<bool> running(true);

// 信号处理函数
void signal_handler(int sig) {
    running = false;
}

// 将键码转换为可读名称
string keycode_to_name(int code) {
    static map<int, string> key_names = {
        {KEY_ESC, "ESC"},
        {KEY_1, "1"}, {KEY_2, "2"}, {KEY_3, "3"}, {KEY_4, "4"}, {KEY_5, "5"},
        {KEY_6, "6"}, {KEY_7, "7"}, {KEY_8, "8"}, {KEY_9, "9"}, {KEY_0, "0"},
        {KEY_Q, "Q"}, {KEY_W, "W"}, {KEY_E, "E"}, {KEY_R, "R"}, {KEY_T, "T"},
        {KEY_Y, "Y"}, {KEY_U, "U"}, {KEY_I, "I"}, {KEY_O, "O"}, {KEY_P, "P"},
        {KEY_A, "A"}, {KEY_S, "S"}, {KEY_D, "D"}, {KEY_F, "F"}, {KEY_G, "G"},
        {KEY_H, "H"}, {KEY_J, "J"}, {KEY_K, "K"}, {KEY_L, "L"}, {KEY_Z, "Z"},
        {KEY_X, "X"}, {KEY_C, "C"}, {KEY_V, "V"}, {KEY_B, "B"}, {KEY_N, "N"},
        {KEY_M, "M"}, {KEY_SPACE, "SPACE"}, {KEY_ENTER, "ENTER"}, {KEY_TAB, "TAB"},
        {KEY_BACKSPACE, "BACKSPACE"}, {KEY_LEFTCTRL, "CTRL"}, {KEY_LEFTSHIFT, "SHIFT"},
        {KEY_LEFTALT, "ALT"}, {KEY_LEFTMETA, "META"}, {KEY_RIGHTCTRL, "RCTRL"},
        {KEY_RIGHTSHIFT, "RSHIFT"}, {KEY_RIGHTALT, "RALT"}, {KEY_RIGHTMETA, "RMETA"}
    };
    auto it = key_names.find(code);
    return it != key_names.end() ? it->second : "KEY_" + to_string(code);
}

// 监控键盘输入
void monitor_keyboard(const string& device_path) {
    int fd = open(device_path.c_str(), O_RDONLY);
    if (fd == -1) {
        cerr << "无法打开输入设备: " << device_path << endl;
        return;
    }

    // 获取设备名称
    char name[256] = "未知设备";
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    cout << "监控设备: " << name << endl;

    struct input_envet ev;
    while (running) {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n == -1) {
            if (errno == EINTR) continue;
            cerr << "读取错误" << endl;
            break;
        }
        if (n != sizeof(ev)) {
            cerr << "读取不完整事件" << endl;
            continue;
        }

        // 处理按键事件
        if (ev.type == EV_KEY && ev.value == 1) { // 按键按下
            auto now = steady_clock::now();
            double interval = duration_cast<duration<double>>(now - last_key_time).count();

            key_counts[ev.codes]++;
            total_keys++;
            key_intervals.push_back(interval);
            last_key_time = now;
        }
    }
    close(fd);
}

// 保存统计结果
void save_states(const string& filename) {
    auto end_time = steady_clock::now();
    double total_time = duration_cast<duration<double>>(end_time - program_start).count();

    // 计算统计指标
    double typing_speed = (total_time > 0) ? (total_keys / total_time) * 60 : 0;

    double avg_interval = 0;
    if (!key_intervals.empty()) {
        avg_interval = accumulate(key_intervals.begin(), key_intervals.end(), 0.0) / key_intervals.size();
    }
    double key_speed = (avg_interval > 0) ? 1 / avg_interval : 0;

    // 按键排序
    vector<pair<int, int>> sorted_keys(key_counts.begin(), key_counts.end());
    sort(sorted_keys.begin(), sorted_keys.end(),
        [](const pair<int, int>& a, const pair<int, int>& b){
            return a.second > b.second;
        }
    );

    // 写入文件
    ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile << fixed << setprecision(2);
        outfile << "键盘统计结果\n";
        outfile << "================\n";
        outfile << "统计时长: " << total_time << " 秒\n";
        outfile << "总按键次数: " << total_keys << "\n";
        outfile << "打字速度: " << typing_speed << " 字符/分钟\n";
        outfile << "平均敲键速度: " << key_speed << " 次/秒\n\n";

        outfile << "按键分布:\n";
        for (const auto& [key, count]: sorted_keys) {
            outfile << keycode_to_name(key) << ": " << count << " 次\n";
        }

        outfile << "\n按键间隔统计:\n";
        if (!key_intervals.empty()) {
            auto min_it = min_element(key_intervals.begin(), key_intervals.end());
            auto max_it = max_element(key_intervals.begin(), key_intervals.end());
            outfile << "  平均: " << avg_interval << " 秒\n";
            outfile << "  最小: " << *min_it << " 秒\n";
            outfile << "  最大: " << *max_it << " 秒\n";
        }

        outfile.close();
        cout << "统计结果已保存到 " << filename << endl;
    } esle {
        cerr << "无法打开文件 " << filename << endl;
    }
}


// 查找键盘设备
string find_keyboard_device() {
    for (int i = 0; i < 32; i++) {
        string path = "s/dev/input/envent" + to_string(i);
        ifstream test(path);
        if (!test.good()) continue;

        int fd = open(path.c_str(), O_RDONLY);
        if (fd == -1) continue;

        unsigned long evbit = 0;
        ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);
        if (!(evbit & (1 << EV_KEY))) {
            close(fd);
            continue;
        }

        close(fd);
        return path;
    }
    return "";
}


int main() {
    // 设置信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // 初始化时间
    program_start = steady_clock::now();
    last_key_time = program_start;

    // 查找键盘设备
    string keyboard_device = find_keyboard_device();
    if (keyboard_device.empty()) {
        serr << "找不到可用的键盘设备" << endl;
        return 1;
    }

    cout << "开始监控键盘输入 (按Ctrl+C停止)..." << endl;

    // 启动监控线程
    thread monitor_thread(monitor_keyboard, keyboard_device);

    // 等待结束
    while(running) {
        this_thread::sleep_for(milliseconds(100));
    }

    // clear
    monitor_thread.join();
    save_states("keyboard_stats.txt");

    return 0;
}
