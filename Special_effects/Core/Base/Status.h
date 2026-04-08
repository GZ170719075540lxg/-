
#pragma once

#include "CoreMinimal.h"
#include "Status.generated.h"

UENUM(BlueprintType)
/**
 * @brief 状态枚举类，用于表示游戏中的各种状态
 * @details 该枚举类定义了游戏中可能出现的各种状态类型，包括正常、死亡、冻结、燃烧、中毒、眩晕等
 * @note 使用uint8作为底层类型，以节省内存空间
 */
enum class EStatus : uint8 // 使用uint8作为枚举的底层类型，占用1字节内存
{
    CMS_Idle UMETA(DisplayName = "Idle"),
    CMS_Walking UMETA(DisplayName = "Walking"),
    CMS_Running UMETA(DisplayName = "Running"),
    CMS_Jumping UMETA(DisplayName = "Jumping"),
    CMS_Falling UMETA(DisplayName = "Falling"),
    CMS_Climbing UMETA(DisplayName = "Climbing"),
    CMS_Swimming UMETA(DisplayName = "Swimming"),
    CMS_Attach UMETA(DisplayName = "Attach"),
    // 可被移动打断状态
    CMS_MoveBreak UMETA(DisplayName = "MoveBreak"),
    // 不可被移动打断状态
    CMS_MoveNotBreak UMETA(DisplayName = "MoveBreak2"),
    // 拉弓状态
    CMS_Arrow UMETA(DisplayName = "Arrow"),
    CMS_Dead UMETA(DisplayName = "Dead")
};
