#pragma once

#include "CoreMinimal.h"
#include "BackPack.generated.h"

UENUM(BlueprintType)
enum class EBackPack : uint8 // 使用uint8作为枚举的底层类型，占用1字节内存
{
    wuqi UMETA(DisplayName = "武器"),
    fangju UMETA(DisplayName = "防具"),
    yaoji UMETA(DisplayName = "药剂"),
    MAX
};
