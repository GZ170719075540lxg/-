// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MyDataTable.h"
#include "Styling/SlateBrush.h"
#include "MyDataTable_iteam.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct RPG_TU2_API FMyDataTable_iteam : public FMyDataTable
{
	GENERATED_BODY()
public:
	// 是否可堆叠
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDataTable_iteam", meta = (DisplayName = "是否可堆叠"))
	bool bStackable;
	// 堆叠数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDataTable_iteam", meta = (DisplayName = "堆叠数量"))
	int32 StackSize;
	// 图标
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDataTable_iteam", meta = (DisplayName = "图标"))
	FSlateBrush Icon;
};
