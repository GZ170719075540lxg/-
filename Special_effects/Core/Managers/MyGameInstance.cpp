// MyGameInstance.cpp
#include "MyGameInstance.h"

#include "Engine/Engine.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "Misc/Paths.h"
#include "HAL/PlatformTime.h"

UMyGameInstance::UMyGameInstance()
    : ClientSocket(nullptr), ServerIP(TEXT("127.0.0.1")), ServerPort(8767), bIsReceiving(false)
{
    // 构造函数中一般不要 Load DataTable
    // 数据表资源在蓝图中指定即可
    UE_LOG(LogTemp, Warning, TEXT("MyGameInstance created"));
    // 不要在构造函数中调用Init()，避免潜在的初始化问题
}

// 处理游戏实例初始化时逻辑
void UMyGameInstance::Init()
{
    Super::Init();

    // 延迟加载WebSockets模块，避免在构造函数中加载
    FTimerManager &LocalTimerManager = GetTimerManager();
    LocalTimerManager.SetTimerForNextTick([this]()
                                     {
        if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
        {
            FModuleManager::Get().LoadModule("WebSockets");
            UE_LOG(LogTemp, Log, TEXT("WebSockets module loaded"));
        } });
}

// 处理游戏实例关闭时逻辑
// 1.关闭所有Websocket连接
void UMyGameInstance::Shutdown()
{
    if (!SocketComponents.IsEmpty())
    {
        TArray<UActorComponent *> Components(SocketComponents);
        for (UActorComponent *Component : Components)
        {
            UTestWebsockets *WebSocketComponent = Cast<UTestWebsockets>(Component);
            WebSocketComponent->CloseWebSocket();
        }
    }
    Super::Shutdown();
}
