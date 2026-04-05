#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
WebSocket 视频流测试脚本
模拟持续发送视频帧到服务器进行测试
"""

import asyncio
import websockets
import cv2
import numpy as np
import time
from datetime import datetime
import argparse


class VideoStreamClient:
    def __init__(self, uri="ws://localhost:8767", fps=30):
        self.uri = uri
        self.fps = fps
        self.frame_interval = 1.0 / fps
        self.websocket = None

    async def connect(self):
        """连接到WebSocket服务器"""
        try:
            self.websocket = await websockets.connect(self.uri)
            print(f"✓ 已连接到 {self.uri}")
            return True
        except Exception as e:
            print(f"✗ 连接失败: {e}")
            return False

    async def send_test_frames(self, duration=30, width=640, height=480):
        """发送测试视频帧"""
        if not self.websocket:
            print("错误: 未建立连接")
            return

        print(f"开始发送测试视频流 ({duration}秒, {self.fps} FPS)")
        print(f"分辨率: {width}x{height}")
        print("按 Ctrl+C 停止发送")

        start_time = time.time()
        frame_count = 0

        try:
            while time.time() - start_time < duration:
                # 生成测试帧
                frame = self.generate_test_frame(width, height, frame_count)

                # 编码为JPEG
                success, encoded_frame = cv2.imencode(
                    '.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 85])
                if not success:
                    print("帧编码失败")
                    continue

                # 发送二进制数据
                await self.websocket.send(encoded_frame.tobytes())
                frame_count += 1

                # 显示进度
                if frame_count % 30 == 0:
                    elapsed = time.time() - start_time
                    actual_fps = frame_count / elapsed
                    print(f"已发送 {frame_count} 帧 ({actual_fps:.1f} FPS)")

                # 控制帧率
                await asyncio.sleep(self.frame_interval)

                # 检查是否有来自服务器的消息
                try:
                    response = await asyncio.wait_for(self.websocket.recv(), timeout=0.001)
                    print(f"服务器响应: {response}")
                except asyncio.TimeoutError:
                    pass

        except KeyboardInterrupt:
            print("\n用户中断发送")
        except Exception as e:
            print(f"发送过程中出错: {e}")
        finally:
            elapsed = time.time() - start_time
            actual_fps = frame_count / elapsed if elapsed > 0 else 0
            print(f"\n发送完成:")
            print(f"  总帧数: {frame_count}")
            print(f"  实际帧率: {actual_fps:.1f} FPS")
            print(f"  运行时间: {elapsed:.2f} 秒")

    def generate_test_frame(self, width, height, frame_num):
        """生成测试视频帧"""
        # 创建黑色背景
        frame = np.zeros((height, width, 3), dtype=np.uint8)

        # 添加彩色渐变背景
        for y in range(height):
            for x in range(width):
                frame[y, x] = [
                    int(128 + 127 * np.sin(frame_num * 0.1 + x * 0.01)),
                    int(128 + 127 * np.sin(frame_num * 0.1 + y * 0.01 + 2)),
                    int(128 + 127 * np.sin(frame_num * 0.1 + (x + y) * 0.005 + 4))
                ]

        # 添加移动的圆形
        center_x = int(width/2 + width/4 * np.sin(frame_num * 0.05))
        center_y = int(height/2 + height/4 * np.cos(frame_num * 0.03))
        radius = 50
        color = (0, 255, 255)  # 黄色

        cv2.circle(frame, (center_x, center_y), radius, color, -1)

        # 添加文字信息
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        text_lines = [
            f"Test Frame #{frame_num}",
            f"Timestamp: {timestamp}",
            f"Resolution: {width}x{height}"
        ]

        for i, text in enumerate(text_lines):
            cv2.putText(frame, text, (20, 40 + i * 30),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

        # 添加网格线
        for i in range(0, width, 50):
            cv2.line(frame, (i, 0), (i, height), (50, 50, 50), 1)
        for i in range(0, height, 50):
            cv2.line(frame, (0, i), (width, i), (50, 50, 50), 1)

        return frame

    async def close(self):
        """关闭连接"""
        if self.websocket:
            await self.websocket.close()
            print("连接已关闭")


async def main():
    parser = argparse.ArgumentParser(description='WebSocket 视频流测试')
    parser.add_argument('--host', default='localhost', help='服务器主机名')
    parser.add_argument('--port', type=int, default=8767, help='服务器端口')
    parser.add_argument('--fps', type=int, default=30, help='帧率')
    parser.add_argument('--duration', type=int, default=30, help='测试持续时间(秒)')
    parser.add_argument('--width', type=int, default=640, help='视频宽度')
    parser.add_argument('--height', type=int, default=480, help='视频高度')

    args = parser.parse_args()

    uri = f"ws://{args.host}:{args.port}"

    print("=" * 50)
    print("WebSocket 视频流测试客户端")
    print("=" * 50)
    print(f"目标服务器: {uri}")
    print(f"测试参数: {args.fps} FPS, {args.duration}秒")
    print(f"分辨率: {args.width}x{args.height}")
    print("=" * 50)

    client = VideoStreamClient(uri, args.fps)

    if await client.connect():
        await client.send_test_frames(args.duration, args.width, args.height)
        await client.close()
    else:
        print("无法连接到服务器，请确保服务器正在运行")


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n测试被用户中断")
    except Exception as e:
        print(f"测试出错: {e}")
