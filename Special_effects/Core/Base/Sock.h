#pragma once

#include "Sock.generated.h"

UENUM(BlueprintType)
enum class ESock : uint8 // 使用uint8作为枚举的底层类型，占用1字节内存
{
    SockR,
    SockL,
    SockL2,
    MAX

};
