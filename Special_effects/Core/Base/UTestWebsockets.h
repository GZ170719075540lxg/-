// UTestWebsockets.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HttpModule.h"
#include "IWebSocket.h"
#include "Misc/Base64.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "../../Characters/RoboticArm.h"

#include "UTestWebsockets.generated.h"

/**
 * WebSocket测试组件
 * 用于测试WebSocket连接和消息收发功能
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPG_TU2_API UTestWebsockets : public UActorComponent
{
    GENERATED_BODY()

public:
    // 构造函数
    UTestWebsockets();

protected:
    // 游戏开始时调用
    virtual void BeginPlay() override;

    // 游戏结束时调用
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    // 每帧调用
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

    /** 是否初始化时进行连接 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebSocket")
    bool InitSocket;

    /** WebSocket连接URL */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebSocket")
    FString WebSocketURL;

    /** 最近一次接收到的信息 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebSocket")
    FString CurrentMessage;

    /** 建立连接的WebSocket对象 */
    TSharedPtr<IWebSocket> WebSocket;

public:
    /**
     * 初始化WebSocket连接
     * @param URL 服务端URL (如: "ws://localhost:8765")
     */
    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void InitWebSocket(const FString &URL);

    /**
     * 关闭WebSocket连接
     */
    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void CloseWebSocket();

    /**
     * 处理接收到的消息
     * @param Message 接收到的消息
     */
    void OnMessage(const FString &Message);

    /**
     * 处理原始数据包
     * @param Data 数据指针
     * @param Size 数据大小
     * @param BytesRemaining 剩余字节数
     */
    void OnPacketReceived(const void *Data, int Size, int BytesRemaining);
    /**
     * 发送到服务端消息
     * @param Message 要发送的消息
     */
    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void SendToServer(const FString &Message);

    /*
     * 发送摄像头保存的图片
     */
    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void SendCameraImage(FString ImagePath);

    /**
     * 发送字节数据
     * @param Data 要发送的字节数据
     */
    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void SendBytes(const TArray<uint8> &Data);
    // 渲染目标2D对象
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebSocket")
    UTextureRenderTarget2D *RenderTarget;
    // 机械臂对象
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebSocket")
    ARoboticArm *RobotArm;
};
