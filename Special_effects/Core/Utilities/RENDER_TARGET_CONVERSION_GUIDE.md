# 渲染目标转图像二进制工具函数使用指南

## 功能概述

本工具库提供了一套完整的函数，用于将 Unreal Engine 中的渲染目标 2D (UTextureRenderTarget2D) 转换为各种图像格式的二进制数据。

## 可用函数

### 1. ConvertRenderTargetToPNG

将渲染目标转换为 PNG 格式的二进制数据

**参数:**

- `RenderTarget`: UTextureRenderTarget2D\* - 要转换的渲染目标

**返回值:**

- `TArray<uint8>` - PNG 格式的二进制数据数组

### 2. ConvertRenderTargetToJPEG

将渲染目标转换为 JPEG 格式的二进制数据

**参数:**

- `RenderTarget`: UTextureRenderTarget2D\* - 要转换的渲染目标
- `Quality`: int32 - JPEG 压缩质量 (1-100，默认 85)

**返回值:**

- `TArray<uint8>` - JPEG 格式的二进制数据数组

### 3. SaveRenderTargetToFile

将渲染目标直接保存为图像文件

**参数:**

- `RenderTarget`: UTextureRenderTarget2D\* - 要保存的渲染目标
- `FilePath`: const FString& - 保存文件路径
- `bSaveAsPNG`: bool - 是否保存为 PNG 格式 (true=PNG, false=JPEG)

**返回值:**

- `bool` - 保存成功返回 true，失败返回 false

### 4. GetRenderTargetPixels

获取渲染目标的原始像素数据

**参数:**

- `RenderTarget`: UTextureRenderTarget2D\* - 要读取的渲染目标

**返回值:**

- `TArray<FColor>` - 像素颜色数据数组

## 使用示例

### Blueprint 使用示例

```blueprint
// 1. 转换为PNG二进制数据
PNGData = ConvertRenderTargetToPNG(MyRenderTarget)

// 2. 转换为JPEG二进制数据 (质量90)
JPEGData = ConvertRenderTargetToJPEG(MyRenderTarget, 90)

// 3. 直接保存为文件
Success = SaveRenderTargetToFile(MyRenderTarget, "C:/Screenshots/MyImage.png", true)

// 4. 获取原始像素数据
PixelData = GetRenderTargetPixels(MyRenderTarget)
```

### C++使用示例

```cpp
// 包含头文件
#include "Core/Utilities/MyBlueprintFunctionLibrary.h"

// 转换为PNG
UTextureRenderTarget2D* RenderTarget = /* 获取你的渲染目标 */;
TArray<uint8> PNGData = UMyBlueprintFunctionLibrary::ConvertRenderTargetToPNG(RenderTarget);

// 转换为JPEG
TArray<uint8> JPEGData = UMyBlueprintFunctionLibrary::ConvertRenderTargetToJPEG(RenderTarget, 85);

// 保存到文件
bool bSaved = UMyBlueprintFunctionLibrary::SaveRenderTargetToFile(
    RenderTarget,
    TEXT("C:/Screenshots/Capture.png"),
    true
);

// 获取像素数据
TArray<FColor> Pixels = UMyBlueprintFunctionLibrary::GetRenderTargetPixels(RenderTarget);
```

## 常见应用场景

### 1. 屏幕截图功能

```cpp
// 截取场景渲染目标
UTextureRenderTarget2D* SceneCaptureTarget = /* 你的场景捕捉组件的渲染目标 */;
TArray<uint8> ScreenshotData = UMyBlueprintFunctionLibrary::ConvertRenderTargetToPNG(SceneCaptureTarget);
```

### 2. UI 元素导出

```cpp
// 导出UI画布为图片
UTextureRenderTarget2D* UICanvasTarget = /* UI渲染目标 */;
UMyBlueprintFunctionLibrary::SaveRenderTargetToFile(UICanvasTarget, TEXT("C:/UI/Screenshot.png"), true);
```

### 3. 实时视频流传输

```cpp
// 将渲染目标转换为二进制数据用于网络传输
TArray<uint8> FrameData = UMyBlueprintFunctionLibrary::ConvertRenderTargetToJPEG(RenderTarget, 75);
// 发送FrameData到网络...
```

### 4. 图像处理和分析

```cpp
// 获取像素数据进行自定义处理
TArray<FColor> PixelData = UMyBlueprintFunctionLibrary::GetRenderTargetPixels(RenderTarget);
// 对像素数据进行分析处理...
```

## 注意事项

1. **确保渲染完成**: 函数会自动处理渲染目标的状态，但在调用前最好确保渲染已经完成

2. **内存管理**: 返回的二进制数据数组会在函数结束时自动清理，如果需要长期保存请复制数据

3. **文件路径**: 保存文件时请确保路径存在且有写入权限

4. **性能考虑**: 频繁调用这些函数可能影响性能，建议在合适的时机调用

5. **错误处理**: 所有函数都有日志输出，可以通过 Output Log 查看详细信息

## 支持的图像格式

- **PNG**: 无损压缩，支持透明度
- **JPEG**: 有损压缩，文件更小，不支持透明度

## 依赖模块

确保项目模块文件(.Build.cs)中包含了以下依赖：

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core", "CoreUObject", "Engine", "ImageWrapper", "ImageCore"
});
```
