
#pragma once

#include "WeaponType.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    none,
    taidao,
    changjian,
    gongjian,
    MAX
};
