// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../../Core/Base/BackPack.h"
#include "MyDataTable.generated.h"

/**
 * 物品信息表
 * 物品表的父类
 *
 *
 *
 */
USTRUCT(BlueprintType)
struct RPG_TU2_API FMyDataTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	// 唯一ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "物品id"))
	int32 ItemID;
	// 名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "物品名称"))
	FString Name;
	// 描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "物品描述"))
	FString Description;
	// 所属类别
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "物品类别"))
	EBackPack ItemType;
};
