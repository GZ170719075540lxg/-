
#pragma once

#include "CoreMinimal.h"
#include "WeaponType.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    none UMETA(DisplayName = "None"),
    quantou UMETA(DisplayName = "拳头"),
    taidao UMETA(DisplayName = "太刀"),
    changjian UMETA(DisplayName = "长剑"),
    gongjian UMETA(DisplayName = "弓箭"),

    MAX
};
