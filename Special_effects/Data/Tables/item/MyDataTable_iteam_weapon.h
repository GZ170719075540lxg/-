// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyDataTable_iteam.h"
#include "../../../Core/Base/WeaponType.h"
#include "Engine/SkeletalMesh.h"
#include "MyDataTable_iteam_weapon.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct RPG_TU2_API FMyDataTable_iteam_weapon : public FMyDataTable_iteam
{
	GENERATED_BODY()
public:
	// 静态网格体
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (DisplayName = "静态网格体"))
	TArray<TObjectPtr<UStaticMesh>> StaticMesh;
	// 武器类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (DisplayName = "武器类型"))
	EWeaponType WeaponType;
	// 骨骼网格体
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (DisplayName = "骨骼网格体"))
	TObjectPtr<USkeletalMesh> SkeletalMesh;
	// 是否有骨骼网格体
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (DisplayName = "是否有骨骼网格体"))
	bool bHasSkeletalMesh;
	// 抛掷物蓝图
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (DisplayName = "抛掷物"))
	TObjectPtr<UClass> ThrowObject;
};
