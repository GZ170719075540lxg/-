// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../../Core/Base/BackPack.h"
#include "Item.generated.h"
/*

可被捡起的物品父类
*/
UCLASS(BlueprintType)
class RPG_TU2_API UItem : public UObject
{
    GENERATED_BODY()
public:
    UItem()
    {
        backtype = EBackPack::wuqi;
        id = 0;
    }

public:
    /*
    物品类别
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (DisplayName = "物品类别"))
    EBackPack backtype;
    /*
    物品id
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (DisplayName = "物品id"))
    int32 id;
};
