// MyGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "TimerManager.h"
#include "../../Data/Tables/MyDataTable.h"
#include "LevelSequence.h"

#include "../../Core/Base/BackPack.h"
#include "../../Core/Base/UTestWebsockets.h"
#include "MyGameInstance.generated.h"

/**
 * 全局游戏实例，用于管理全局资源和配置
 */
UCLASS()
class UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UMyGameInstance();
    /** 蓝图中指定的物品表 DataTable */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data", meta = (ToolTip = "物品总览表"))
    UDataTable *ItemTable;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data", meta = (ToolTip = "物品详细表"))
    TMap<EBackPack, UDataTable *> ItemDetailTable;
    // 存储图片
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data", meta = (ToolTip = "物品图片表"))
    TMap<EBackPack, FSlateBrush> ItemTexture;

private:
    // 网络连接相关成员变量
    FSocket *ClientSocket;
    FString ServerIP;
    int32 ServerPort;
    bool bIsReceiving;

public:
    // WebSocket组件数组
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UTestWebsockets *> SocketComponents;

    // 处理游戏实例关闭时逻辑
    // 1.关闭所有Websocket连接
    virtual void Shutdown() override;

    // 处理游戏实例初始化时逻辑
    virtual void Init() override;
};