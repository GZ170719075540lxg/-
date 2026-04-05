#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试OpenCV图片显示功能
"""

import cv2
import numpy as np
import asyncio
import websockets
import base64
from pathlib import Path


async def test_send_image():
    """测试向WebSocket服务器发送图片"""
    try:
        # 连接到WebSocket服务器
        uri = "ws://localhost:8767"
        async with websockets.connect(uri) as websocket:
            print(f"已连接到 {uri}")

            # 查找测试图片
            test_images = list(Path(".").glob("*.png")) + \
                list(Path(".").glob("*.jpg"))

            if test_images:
                # 发送找到的第一张图片
                image_path = test_images[0]
                print(f"发送图片: {image_path}")

                with open(image_path, "rb") as f:
                    image_data = f.read()

                # 发送二进制图片数据
                await websocket.send(image_data)
                print(f"已发送 {len(image_data)} 字节的图片数据")

                # 等待服务器响应
                response = await websocket.recv()
                print(f"服务器响应: {response}")

            else:
                print("未找到测试图片文件")

    except Exception as e:
        print(f"测试失败: {e}")


def test_local_opencv():
    """本地测试OpenCV显示功能"""
    print("测试本地OpenCV显示...")

    # 创建一个简单的测试图片
    img = np.zeros((300, 400, 3), dtype=np.uint8)
    img[:, :] = [100, 150, 200]  # 蓝色背景

    # 添加一些文字
    cv2.putText(img, "Test Image", (50, 150),
                cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

    # 显示图片
    cv2.imshow("Local Test", img)
    print("按任意键关闭测试窗口...")
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    print("本地测试完成")


if __name__ == "__main__":
    print("OpenCV显示功能测试")
    print("=" * 30)

    # 首先测试本地OpenCV功能
    test_local_opencv()

    # 然后测试WebSocket发送
    print("\n开始WebSocket测试...")
    asyncio.run(test_send_image())
