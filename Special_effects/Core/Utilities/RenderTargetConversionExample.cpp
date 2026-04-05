// 渲染目标转换工具函数使用示例
// 此文件展示了如何在实际项目中使用MyBlueprintFunctionLibrary中的图像转换功能

// 渲染目标转换工具函数使用示例
// 此文件展示了如何在实际项目中使用MyBlueprintFunctionLibrary中的图像转换功能

#include "MyBlueprintFunctionLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Engine.h"

// 注意：此示例文件主要用于演示代码结构，实际使用时需要根据具体项目调整

// 示例1: 屏幕截图功能
/*
void TakeScreenshotExample()
{
    // 假设你有一个场景捕捉组件
    ASceneCapture2D* SceneCapture = nullptr; // 实际使用时需要获取有效的场景捕捉Actor
    if (SceneCapture && SceneCapture->GetCaptureComponent2D())
    {
        UTextureRenderTarget2D* RenderTarget = SceneCapture->GetCaptureComponent2D()->TextureTarget;

        if (RenderTarget)
        {
            // 转换为PNG格式
            TArray<uint8> PNGData = UMyBlueprintFunctionLibrary::ConvertRenderTargetToPNG(RenderTarget);

            if (PNGData.Num() > 0)
            {
                UE_LOG(LogTemp, Log, TEXT("成功生成PNG截图，大小: %d 字节"), PNGData.Num());

                // 保存到文件
                FString FilePath = FString::Printf(TEXT("%s/Screenshots/Screenshot_%s.png"),
                                                   *FPaths::ProjectSavedDir(),
                                                   *FDateTime::Now().ToString(TEXT("yyyyMMdd_HHmmss")));

                if (UMyBlueprintFunctionLibrary::SaveRenderTargetToFile(RenderTarget, FilePath, true))
                {
                    UE_LOG(LogTemp, Log, TEXT("截图已保存到: %s"), *FilePath);
                }
            }
        }
    }
}

// 示例2: 实时视频流准备
TArray<uint8> PrepareVideoFrame(UTextureRenderTarget2D *RenderTarget, int32 Quality = 75)
{
    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("PrepareVideoFrame: RenderTarget is null"));
        return TArray<uint8>();
    }

    // 转换为JPEG以减小数据大小
    TArray<uint8> FrameData = UMyBlueprintFunctionLibrary::ConvertRenderTargetToJPEG(RenderTarget, Quality);

    if (FrameData.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("视频帧准备完成，大小: %d 字节"), FrameData.Num());
    }

    return FrameData;
}

// 示例3: 图像数据分析
void AnalyzeRenderTargetPixels(UTextureRenderTarget2D *RenderTarget)
{
    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnalyzeRenderTargetPixels: RenderTarget is null"));
        return;
    }

    TArray<FColor> Pixels = UMyBlueprintFunctionLibrary::GetRenderTargetPixels(RenderTarget);

    if (Pixels.Num() > 0)
    {
        // 简单的像素分析示例
        int32 BrightPixels = 0;
        int32 DarkPixels = 0;

        for (const FColor &Pixel : Pixels)
        {
            // 计算亮度 (简化算法)
            float Brightness = (Pixel.R + Pixel.G + Pixel.B) / 3.0f;
            if (Brightness > 128)
            {
                BrightPixels++;
            }
            else
            {
                DarkPixels++;
            }
        }

        UE_LOG(LogTemp, Log, TEXT("图像分析结果 - 总像素: %d, 明亮像素: %d, 暗像素: %d"),
               Pixels.Num(), BrightPixels, DarkPixels);
    }
}

// 示例4: 批量处理多个渲染目标
void BatchProcessRenderTargets(const TArray<UTextureRenderTarget2D *> &Targets)
{
    for (int32 i = 0; i < Targets.Num(); ++i)
    {
        UTextureRenderTarget2D *Target = Targets[i];
        if (!Target)
            continue;

        FString FileName = FString::Printf(TEXT("BatchOutput_%03d.png"), i);
        FString FullPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("BatchOutput"), FileName);

        if (UMyBlueprintFunctionLibrary::SaveRenderTargetToFile(Target, FullPath, true))
        {
            UE_LOG(LogTemp, Log, TEXT("批量处理: %s 保存成功"), *FileName);
        }
    }
}

// 示例5: 与WebSocket集成 (基于现有WebSocket系统)
// /*
// void SendRenderTargetOverWebSocket(UTextureRenderTarget2D* RenderTarget)
// {
//     if (!RenderTarget)
//     {
//         UE_LOG(LogTemp, Warning, TEXT("SendRenderTargetOverWebSocket: RenderTarget is null"));
//         return;
//     }
//
//     // 转换为较小的JPEG格式用于网络传输
//     TArray<uint8> ImageData = UMyBlueprintFunctionLibrary::ConvertRenderTargetToJPEG(RenderTarget, 60);
//
//     if (ImageData.Num() > 0)
//     {
//         // 这里可以集成到现有的WebSocket系统
//         // 例如: YourWebSocketClient->SendBinaryData(ImageData);
//
//         UE_LOG(LogTemp, Log, TEXT("准备通过WebSocket发送图像数据，大小: %d 字节"), ImageData.Num());
//     }
// }
// */