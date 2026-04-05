// UTestWebsockets.cpp
#include "UTestWebsockets.h"
#include "WebSocketsModule.h"
#include "Engine/TextureRenderTarget2D.h"
#include "../Utilities/MyBlueprintFunctionLibrary.h"
#include "../../Characters/RoboticArm.h"
#include "Engine.h"

UTestWebsockets::UTestWebsockets()
    : InitSocket(false), WebSocketURL(TEXT("ws://localhost:8767")), RobotArm(nullptr)
{
    // 设置组件在游戏开始时初始化并在每帧更新
    PrimaryComponentTick.bCanEverTick = true;
}

void UTestWebsockets::BeginPlay()
{

    Super::BeginPlay();

    // 如果设置了默认连接

    UE_LOG(LogTemp, Log, TEXT("UTestWebsockets created"));
    InitWebSocket(WebSocketURL);
}

void UTestWebsockets::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 清理WebSocket连接
    if (WebSocket.IsValid())
    {
        WebSocket->Close();
        WebSocket.Reset();
        UE_LOG(LogTemp, Log, TEXT("UTestWebsockets cleaned up"));
    }

    Super::EndPlay(EndPlayReason);
}

void UTestWebsockets::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        // 组件每帧更新逻辑（如果需要）
        if (RenderTarget)
        {
            SendBytes(UMyBlueprintFunctionLibrary::ConvertRenderTargetToJPEG(RenderTarget, 60));
        }
    }

    // 组件每帧更新逻辑（如果需要）
}

void UTestWebsockets::InitWebSocket(const FString &URL)
{
    UE_LOG(LogTemp, Log, TEXT("InitWebSocket: %s"), *URL);
    if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    {
        FModuleManager::Get().LoadModule("WebSockets");
    }

    TArray<FString> Protocols;
    Protocols.Add(TEXT("ws"));

    WebSocket = FWebSocketsModule::Get().CreateWebSocket(URL, Protocols);
    WebSocketURL = URL;
    InitSocket = true;

    // 连接成功回调
    WebSocket->OnConnected().AddLambda([this, URL]()
                                       {
			const FString Msg = TEXT("{\"type\":\"ue_connect\",\"project_name\":\"RPG_TU2\",\"message\":\"UE客户端已连接\"}");
			WebSocket->Send(Msg);
			UE_LOG(LogTemp, Log, TEXT("WebSocket连接成功: %s"), *URL); });

    // 连接失败回调
    WebSocket->OnConnectionError().AddLambda([this](const FString &Error)
                                             { UE_LOG(LogTemp, Error, TEXT("WebSocket连接错误: %s"), *Error); });

    // 消息接收回调
    WebSocket->OnMessage().AddLambda([this](const FString &Message)
                                     {
                                         OnMessage(Message);
                                         // 调用机械臂
                                         RobotArm->ExecuteCommand(Message); });

    // 原始数据接收回调
    WebSocket->OnRawMessage().AddLambda([this](const void *Data, SIZE_T Size, SIZE_T BytesRemaining)
                                        { OnPacketReceived(Data, static_cast<int>(Size), static_cast<int>(BytesRemaining)); });

    // 连接关闭回调
    WebSocket->OnClosed().AddLambda([this](int32 StatusCode, const FString &Reason, bool bWasClean)
                                    { UE_LOG(LogTemp, Log, TEXT("WebSocket连接关闭 - 状态码: %d, 原因: %s, 干净关闭: %s"),
                                             StatusCode, *Reason, bWasClean ? TEXT("是") : TEXT("否")); });

    WebSocket->Connect();
}

void UTestWebsockets::CloseWebSocket()
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        // 发送断开连接消息
        const FString DisconnectionMsg = TEXT("{\"type\":\"ue_disconnect\",\"project_name\":\"RPG_TU2\",\"message\":\"UE客户端已断开连接\"}");
        WebSocket->Send(DisconnectionMsg);

        WebSocket->Close();
        UE_LOG(LogTemp, Log, TEXT("WebSocket连接已关闭"));
    }
}

void UTestWebsockets::OnMessage(const FString &Message)
{
    // 检查组件是否仍然有效
    if (!IsValid(this))
    {
        UE_LOG(LogTemp, Warning, TEXT("WebSocket消息回调: 组件已失效"));
        return;
    }

    if (!Message.IsEmpty())
    {
        CurrentMessage = Message;
        UE_LOG(LogTemp, Log, TEXT("收到WebSocket消息: %s"), *Message);

        // 在屏幕上显示消息
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                                             FString::Printf(TEXT("WebSocket消息[%d]: %s"), Message.Len(), *Message));
        }
    }
}

void UTestWebsockets::OnPacketReceived(const void *Data, int Size, int BytesRemaining)
{
    // 检查组件是否仍然有效
    if (!IsValid(this))
    {
        UE_LOG(LogTemp, Warning, TEXT("WebSocket数据包回调: 组件已失效"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("收到原始数据包 - 大小: %d, 剩余: %d"), Size, BytesRemaining);

    if (GEngine && Size > 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue,
                                         FString::Printf(TEXT("原始数据[%d字节]"), Size));
    }
}

/**
 * 发送到服务端消息
 * @param Message 要发送的消息
 */
void UTestWebsockets::SendToServer(const FString &Message)
{
    if (!WebSocket.IsValid() || !WebSocket->IsConnected())
    {
        UE_LOG(LogTemp, Warning, TEXT("WebSocket未连接"));
        return;
    }
    // 2. 正确的线程创建方式
    TSharedRef<IWebSocket> SocketRef = WebSocket.ToSharedRef();
    FString MessageCopy = Message;

    // 使用FRunnableThread而不是FPlatformProcess::CreateThread
    class FWebSocketSendTask : public FRunnable
    {
    public:
        FWebSocketSendTask(TSharedRef<IWebSocket> InSocket, FString InMessage)
            : Socket(InSocket), Message(InMessage) {}

        virtual uint32 Run() override
        {
            // 延迟一点发送，避免重复
            FPlatformProcess::Sleep(0.1f);

            if (Socket->IsConnected())
            {
                Socket->Send(Message);
                UE_LOG(LogTemp, Verbose, TEXT("后台线程发送完成"));
            }
            return 0;
        }

    private:
        TSharedRef<IWebSocket> Socket;
        FString Message;
    };

    // 创建并启动线程
    FRunnableThread::Create(
        new FWebSocketSendTask(SocketRef, MessageCopy),
        TEXT("WebSocketSendThread"),
        0,          // 默认堆栈大小
        TPri_Normal // 线程优先级
    );
}

/**
 * 发送摄像头保存的图片
 * @param ImagePath 图片路径
 */
void UTestWebsockets::SendCameraImage(FString ImagePath)
{
    // 添加详细的前置检查
    UE_LOG(LogTemp, Log, TEXT("准备发送图片 - 路径: %s"), *ImagePath);

    // 1. 检查文件是否存在
    if (!FPaths::FileExists(ImagePath))
    {
        UE_LOG(LogTemp, Error, TEXT("❌ 图片文件不存在: %s"), *ImagePath);
        return;
    }

    // 2. 检查WebSocket连接状态
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        UE_LOG(LogTemp, Log, TEXT("✅ WebSocket连接正常，开始处理图片"));
        // 读取文件
        TArray<uint8> ImageBytes;
        if (!FFileHelper::LoadFileToArray(ImageBytes, *ImagePath))
        {
            UE_LOG(LogTemp, Error, TEXT("❌ 无法加载图片文件: %s"), *ImagePath);
            return;
        }
        UE_LOG(LogTemp, Log, TEXT("图片文件加载成功 - 大小: %d 字节"), ImageBytes.Num());
        WebSocket->Send(ImageBytes.GetData(), ImageBytes.Num(), true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ WebSocket未连接，无法发送图片"));

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
                                             TEXT("❌ WebSocket未连接，图片发送失败"));
        }
    }
}
// ... existing code ...

/*
 * 发送字节数据
 * @param Data 要发送的字节数据
 * @param Size 数据大小
 */
void UTestWebsockets::SendBytes(const TArray<uint8> &Data)
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        WebSocket->Send(Data.GetData(), Data.Num(), true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ WebSocket未连接，无法发送数据"));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
                                             TEXT("❌ WebSocket未连接，数据发送失败"));
        }
    }
}
