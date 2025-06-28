from pynput import keyboard
import time
from collections import defaultdict
import sqlite3
import matplotlib.pyplot as plt
import threading
import queue
import sys
import logging

class KeyboardMonitor:
    def __init__(self):
        # 初始化数据结构
        self.key_counts = defaultdict(int)
        self.key_durations = {} # 按键持续时间 {key: [press_time, release_time]}
        self.word_start = None
        self.word_count = 0
        self.last_key_time = time.time()
        self.event_queue = queue.Queue()    # 异步事件队列
        self.running = True
        self.listener = None  # 显式保存监听器引用

        # 初始化数据库
        self.db_con = sqlite3.connect("keyboard_stats.db")
        self._init_db()

        # 热键组合设置
        self.current_keys = set()
        self.EXIT_KEYS = {
            keyboard.Key.alt_l,
            keyboard.Key.ctrl_l,
            keyboard.Key.shift, 'a'
        }
        self.SHOW_KEYS = {
            keyboard.Key.alt_l,
            keyboard.Key.ctrl_l,
            keyboard.Key.shift,
            keyboard.Key.space
        }

    def _init_db(self):
        cursor = self.db_con.cursor()
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS key_events (
                id INTEGER PRIMARY KEY,
                key TEXT NOT NULL,
                event_type TEXT CHECK(event_type IN ('press', 'release')),
                timestamp REAL NOT NULL
            )
        ''')
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS daily_stats (
                date TEXT PRIMARY KEY,
                total_keys INTEGER DEFAULT 0,
                words INTEGER DEFAULT 0,
                kpm REAL DEFAULT 0.0
            )
        ''')
        self.db_con.commit()

    def _process_events(self):
        # 异步处理事件队列
        logging.info("事件处理线程启动")
        print("self.running: ", self.running)
        print("self.event_queue.empty(): ", self.event_queue.empty())
        print("self.event_queue: ", self.event_queue)
        while self.running:
            try:
                event = self.event_queue.get()
                print("event: ", event)
                event_type, key, timestamp = event
                print("event_type: ", event_type)
                print("key: ", key)
                print("timestamp: ", timestamp)

                # 记录到数据库
                cursor = self.db_con.cursor()
                cursor.execute(
                    "INSERT INTO key_events (key, event_type, timestamp) VALUES (?, ?, ?)",
                    (str(key), event_type, timestamp)
                )

                # 处理按键统计
                if event_type == "press":
                    self._handle_press(key, timestamp)
                else:
                    self._handle_release(key, timestamp)
                self.event_queue.task_done()
                print("aaa\nohuo\noehuosi")
            except queue.Empty:
                continue  # 正常等待新事件
            except Exception as e:
                logging.error(f"事件处理错误: {str(e)}")

        self.db_con.commit()
        logging.info("事件处理线程退出")

    def _handle_press(self, key, timestamp):
        # 处理按键按下事件
        key_str = self._format_key(key)

        # 更新当前按键集合
        self.current_keys.add(key_str)
        self.key_counts[key_str] += 1

        # 记录按下时间（用于计算持续时间）
        self.key_durations[key_str] = [timestamp, None]

        # 单词边界检测（非字符键结束单词）
        if self._is_char_key(key):
            if self.word_start is None:
                self.word_start = timestamp
        else:
            self._finalize_word(timestamp)
        self.last_key_time = timestamp

    def _handle_release(self, key, timestamp):
        # 处理按键释放事件
        key_str = self._format_key(key)

        # 更新释放时间
        if key_str in self.key_durations and self.key_durations[key_str][1] is None:
            self.key_durations[key_str][1] = timestamp
        
        # 检查热键组合
        if self._check_hotkey(self.EXIT_KEYS):
            print("\n检测到停止热键(ALT+CTRL+SHIFT+A)，停止监控...")
            self.stop()
        elif self._check_hotkey(self.SHOW_KEYS):
            self.show_stats()
        
        self.current_keys.discard(key_str)

    def _format_key(self, key):
        # 标准化按键标识（不区分大小写）
        try:
            # 统一转为小写，使热键检测不区分大小写
            return key.char.lower() if key.char else key.name
        except AttributeError:
            return str(key)
    
    def _is_char_key(self, key):
        # 判断是否为字符键
        return hasattr(key, 'char') and key.char is not None

    def _finalize_word(self, timestamp):
        # 完成单词计数
        if self.word_start and (timestamp - self.word_start) < 5.0:  # 5秒内连续输入计一词
            self.word_count += 1
        self.word_start = None

    def _check_hotkey(self, target_keys):
        # 检查当前按键是否匹配目标热键
        return all(k in self.current_keys for k in target_keys)

    def start_listener(self):
        # 显式启动监听器
        logging.info("正在启动键盘监听器")
        self.listener = keyboard.Listener(
            on_press=self.on_press,
            on_release=self.on_release
        )
        self.listener.start()
        logging.info("监听器已启动")

    def on_press(self, key):
        # 按键按下回调
        logging.debug(f"按下捕获: {key}")
        self.event_queue.put(('press', key, time.time()))

    def on_release(self, key):
        # 按键释放回调
        logging.debug(f"释放捕获: {key}")
        self.event_queue.put(('release', key, time.time()))
        if key == keyboard.Key.esc:  # 单独ESC键退出
            self.stop()
            return False

    def calculate_speed(self):
        # 计算实时打字速度
        current_time = time.time()
        elapsed_min = (current_time - self.start_time) / 60.0

        # 键速 (Keys Per Minute)
        total_keys = sum(self.key_counts.values())
        kpm = total_keys / elapsed_min if elapsed_min > 0 else 0

        # 字速 (Words Per Minute)
        wpm = self.word_count / elapsed_min if elapsed_min > 0 else 0

        return kpm, wpm

    def show_stats(self):
        # 显示统计数据并生成可视化
        kpm, wpm = self.calculate_speed()
        print(f"\n{'='*40}")
        print(f"运行时间: {time.strftime('%H:%M:%S', time.gmtime(time.time()-self.start_time))}")
        print(f"总按键数: {sum(self.key_counts.values())}")
        print(f"当前键速: {kpm:.1f} KPM | 打字速度: {wpm:.1f} WPM")
        print(f"按键分布 TOP5: {sorted(self.key_counts.items(), key=lambda x: x[1], reverse=True)[:5]}")
        print('='*40)

        # 保存每日统计
        cursor = self.db_con.cursor()
        today = time.strftime("%Y-%m-%d")
        cursor.execute('''
            INSERT OR REPLACE INTO daily_stats (date, total_keys, words, kpm)
            VALUES (?, ?, ?, ?)
        ''', (today, sum(self.key_counts.values()), self.word_count, kpm))
        self.db_con.commit()
        
        # 生成可视化图表
        self.generate_visualization()

    def generate_visualization(self):
        # 生成可视化图表

        # 按键分布饼图
        top_keys = sorted(self.key_counts.items(), key=lambda x: x[1], reverse=True)[:8]
        labels = [k[0] for k in top_keys]
        sizes = [k[1] for k in top_keys]
        
        plt.figure(figsize=(12, 8))
        plt.subplot(2, 2, 1)
        plt.pie(sizes, labels=labels, autopct='%1.1f%%', startangle=90)
        plt.title('按键频率分布')

        # 速度变化曲线（需历史数据）
        cursor = self.db_con.cursor()
        cursor.execute("SELECT date, kpm, wpm FROM daily_stats ORDER BY date DESC LIMIT 7")
        rows = cursor.fetchall()
        
        if rows:
            dates = [row[0][5:] for row in rows][::-1]  # 取MM-DD
            kpms = [row[1] for row in rows][::-1]
            wpms = [row[2] for row in rows][::-1]
            
            plt.subplot(2, 2, 2)
            plt.plot(dates, kpms, 'o-', label='键速(KPM)')
            plt.plot(dates, wpms, 's-', label='字速(WPM)')
            plt.xlabel('日期')
            plt.ylabel('速度')
            plt.legend()
            plt.title('速度变化趋势')
        
        plt.tight_layout()
        plt.savefig('keyboard_stats.png')
        print("可视化图表已保存为 keyboard_stats.png")

    def start(self):
        # 启动监听
        self.start_time = time.time()
        print("键盘监控已启动，使用以下热键操作：")
        print("  ESC - 单独退出")
        print("  ALT+CTRL+SHIFT+SPACE - 查看统计")
        print("  ALT+CTRL+SHIFT+A - 停止监控")

        # 启动事件处理线程
        self.process_thread = threading.Thread(
            target=self._process_events,
            daemon=True
        )
        self.process_thread.start()

        # 启动键盘监听
        self.start_listener()

        # 主线程监控状态
        try:
            while self.running:
                time.sleep(0.5)
                qsize = self.event_queue.qsize()
                print(f"事件队列状态: 大小={qsize}, 空={self.event_queue.empty()}")
                if qsize > 0:
                    print(f"最新事件: {self.event_queue.queue[-1]}")
        except KeyboardInterrupt:
            self.stop()

    def stop(self):
        # 停止监听
        logging.info("停止监控...")
        self.running = False

        if self.listener:
            self.listener.stop()

        self.process_thread.join(timeout=2.0)
        # self.show_stats()
        self.db_con.close()
        print("监控已停止")
        sys.exit(0)

if __name__ == "__main__":
    print("="*50)
    print("在Linux/macOS上请使用sudo运行，在Windows上请用管理员身份运行")
    print("="*50)
    monitor = KeyboardMonitor()
    try:
        monitor.start()
    except Exception as e:
        logging.critical(f"致命错误: {str(e)}")
        monitor.stop()
