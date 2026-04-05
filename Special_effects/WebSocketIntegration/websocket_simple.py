# websocket_simple.py - 视频流展示版本

import asyncio
import websockets
import threading
import sys
import struct
import cv2
import numpy as np
from datetime import datetime
import time
import queue

clients = set()
main_loop = None


class VideoStreamProcessor:
    """视频流处理器 - 管理视频帧缓冲和实时显示"""

    def __init__(self, buffer_size=5, target_fps=30):
        self.buffer_size = buffer_size
        self.target_fps = target_fps
        self.frame_buffer = queue.Queue(maxsize=buffer_size)
        self.running = False
        self.display_thread = None
        self.window_name = "WebSocket Video Stream"
        self.received_frames = 0
        self.displayed_frames = 0
        self.start_time = time.time()

    def add_frame(self, frame):
        """添加新的视频帧到缓冲区"""
        self.received_frames += 1

        # 如果缓冲区满了，移除最旧的帧
        if self.frame_buffer.full():
            try:
                self.frame_buffer.get_nowait()
            except queue.Empty:
                pass

        try:
            self.frame_buffer.put_nowait(frame.copy())
        except queue.Full:
            pass

    def run_display(self):
        """运行视频显示循环"""
        self.running = True
        last_frame_time = time.time()
        frame_interval = 1.0 / self.target_fps

        while self.running:
            current_time = time.time()

            # 控制帧率
            if current_time - last_frame_time < frame_interval:
                time.sleep(0.001)  # 短暂休眠
                continue

            try:
                # 从缓冲区获取帧
                if not self.frame_buffer.empty():
                    frame = self.frame_buffer.get_nowait()
                    self.displayed_frames += 1

                    # 显示帧率信息
                    if self.displayed_frames % 30 == 0:
                        elapsed = time.time() - self.start_time
                        actual_fps = self.displayed_frames / elapsed
                        print(f"显示帧率: {actual_fps:.2f} FPS")

                    # 显示视频帧
                    cv2.imshow(self.window_name, frame)
                    last_frame_time = current_time

                    # 检查窗口是否被关闭
                    if cv2.getWindowProperty(self.window_name, cv2.WND_PROP_VISIBLE) < 1:
                        self.running = False
                        break

            except queue.Empty:
                # 缓冲区为空，短暂等待
                time.sleep(0.01)
                continue
            except Exception as e:
                print(f"显示错误: {e}")
                break

            # 处理键盘输入
            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                self.running = False
                break
            elif key == ord(' '):  # 空格键暂停
                cv2.waitKey(0)

        # 清理资源
        cv2.destroyAllWindows()

    def start(self):
        """启动视频显示线程"""
        if self.display_thread is None or not self.display_thread.is_alive():
            self.display_thread = threading.Thread(
                target=self.run_display, daemon=True)
            self.display_thread.start()
            print(f"视频显示已启动 - 目标帧率: {self.target_fps} FPS")
        return self.display_thread

    def stop(self):
        """停止视频显示"""
        self.running = False
        if self.display_thread:
            self.display_thread.join(timeout=2.0)
        cv2.destroyAllWindows()

    def get_stats(self):
        """获取统计信息"""
        elapsed = time.time() - self.start_time
        actual_fps = self.displayed_frames / elapsed if elapsed > 0 else 0
        return {
            'received_frames': self.received_frames,
            'displayed_frames': self.displayed_frames,
            'buffer_size': self.frame_buffer.qsize(),
            'actual_fps': actual_fps,
            'elapsed_time': elapsed
        }


# 全局视频流处理器实例
video_processor = None


async def handle_client(websocket):
    """处理单个客户端连接的核心函数"""
    global main_loop, video_processor
    clients.add(websocket)
    main_loop = asyncio.get_running_loop()

    # 初始化视频处理器
    if video_processor is None:
        video_processor = VideoStreamProcessor(buffer_size=5, target_fps=30)
        video_processor.start()

    # 控制台输入处理线程
    def console_input_handler():
        while True:
            try:
                message = input("请输入要发送的消息 (输入'quit'退出, 'stats'查看统计): ")
                if message.lower() == 'quit':
                    asyncio.run_coroutine_threadsafe(
                        shutdown_server(), main_loop)
                    break
                elif message.lower() == 'stats':
                    if video_processor:
                        stats = video_processor.get_stats()
                        print(f"\n=== 视频流统计 ===")
                        print(f"接收帧数: {stats['received_frames']}")
                        print(f"显示帧数: {stats['displayed_frames']}")
                        print(f"缓冲区大小: {stats['buffer_size']}")
                        print(f"实际帧率: {stats['actual_fps']:.2f} FPS")
                        print(f"运行时间: {stats['elapsed_time']:.2f} 秒")
                        print(f"==================\n")
                elif message.strip():
                    asyncio.run_coroutine_threadsafe(
                        broadcast_message(f"服务器消息: {message}"),
                        main_loop
                    )
            except Exception as e:
                print(f"控制台输入错误: {e}")
                break

    if not hasattr(handle_client, "input_thread_started"):
        thread = threading.Thread(target=console_input_handler, daemon=True)
        thread.start()
        handle_client.input_thread_started = True

    try:
        print("处理消息")
        # 同时处理文本和二进制消息
        async for message in websocket:
            if isinstance(message, str):
                # 文本消息处理
                print(f"收到文本消息: {message}")
                await broadcast_message(f"服务器已收到: {message}")
            else:
                # 二进制消息处理（视频帧）
                await handle_video_frame(websocket, message)

    except websockets.ConnectionClosed:
        print("客户端断开连接")
    finally:
        clients.remove(websocket)
        # 停止视频处理器
        if video_processor and len(clients) == 0:
            video_processor.stop()
            video_processor = None


async def handle_video_frame(websocket, binary_data):
    """处理视频帧数据"""
    global video_processor

    try:
        data = bytes(binary_data)
        # print(f"收到视频帧数据: {len(data)} 字节")  # 可以取消注释用于调试

        # 解码图像
        nparr = np.frombuffer(data, np.uint8)
        frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

        if frame is not None:
            # 添加时间戳水印
            timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
            cv2.putText(frame, f"Frame: {video_processor.received_frames + 1} | {timestamp}",
                        (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)

            # 添加帧率信息
            if video_processor.received_frames > 0:
                stats = video_processor.get_stats()
                cv2.putText(frame, f"FPS: {stats['actual_fps']:.1f}",
                            (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)

            # 添加到视频处理器
            video_processor.add_frame(frame)

            # 定期发送确认消息
            if video_processor.received_frames % 60 == 0:  # 每60帧发送一次确认
                stats = video_processor.get_stats()
                await websocket.send(f"已接收 {stats['received_frames']} 帧, 显示 {stats['displayed_frames']} 帧, 当前FPS: {stats['actual_fps']:.1f}")

        else:
            print("警告: 无法解码视频帧数据")
            await websocket.send("视频帧解码失败")

    except Exception as e:
        error_msg = f"视频帧处理错误: {str(e)}"
        print(error_msg)
        await websocket.send(error_msg)


async def broadcast_message(message):
    """广播消息给所有客户端"""
    if clients:
        # 现代websockets版本中，直接尝试发送，捕获连接关闭异常
        tasks = []
        for client in clients:
            try:
                # 检查连接是否仍然活跃
                if hasattr(client, 'open') and client.open:
                    tasks.append(client.send(message))
                elif not hasattr(client, 'open'):  # 如果没有open属性，直接尝试发送
                    tasks.append(client.send(message))
            except Exception:
                # 客户端可能已经断开，从集合中移除
                clients.discard(client)
                continue

        if tasks:
            try:
                # 执行发送任务，忽略个别连接的异常
                results = await asyncio.gather(*tasks, return_exceptions=True)
                successful_sends = sum(
                    1 for result in results if not isinstance(result, Exception))
                print(f"已向 {successful_sends}/{len(tasks)} 个客户端广播消息: {message}")

                # 移除发送失败的客户端
                for i, result in enumerate(results):
                    if isinstance(result, Exception):
                        clients.discard(list(clients)[i])

            except Exception as e:
                print(f"广播消息时出错: {e}")


async def shutdown_server():
    """关闭服务器"""
    print("正在关闭服务器...")
    for client in list(clients):
        await client.close()
    clients.clear()
    asyncio.get_running_loop().stop()


async def main():
    # 10MB最大消息大小
    async with websockets.serve(handle_client, "localhost", 8767, max_size=10*1024*1024):
        print("=" * 50)
        print("WebSocket 视频流服务器已启动")
        print("=" * 50)
        print(f"监听地址: ws://localhost:8767")
        print("功能特性:")
        print("  ✓ 支持实时视频流传输")
        print("  ✓ 视频帧缓冲和流畅播放")
        print("  ✓ 实时帧率显示和统计")
        print("  ✓ 时间戳水印")
        print("  ✓ 支持文本和二进制消息")
        print()
        print("控制台命令:")
        print("  输入消息 - 广播给所有客户端")
        print("  'stats'  - 查看视频流统计信息")
        print("  'quit'   - 关闭服务器")
        print("视频窗口控制:")
        print("  'q'      - 退出视频显示")
        print("  '空格'   - 暂停/继续播放")
        print("=" * 50)

        try:
            await asyncio.Future()
        except asyncio.CancelledError:
            pass

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n服务器被用户中断")
    except Exception as e:
        print(f"服务器运行出错: {e}")
    finally:
        print("服务器已停止")
