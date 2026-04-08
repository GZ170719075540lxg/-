// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/SkeletalMesh.h"
#include "../../Core/Base/WeaponType.h"
#include "MyWeapons.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPG_TU2_API UMyWeapons : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMyWeapons();
	// 析构

	// 武器类型

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器", meta = (DisplayName = "武器类型"))
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器部分", meta = (DisplayName = "武器"))
	TArray<TObjectPtr<UStaticMesh>> WeaponMesh1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器部分", meta = (DisplayName = "武器骨骼网格体"))
	TObjectPtr<USkeletalMesh> WeaponMeshs;
	// 是否是网格体武器
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器部分", meta = (DisplayName = "是否是网格体武器"))
	bool isMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器部分", meta = (DisplayName = "抛掷物"))
	TObjectPtr<UClass> ThrowObject;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	// 发射弓箭
	void DrawBowMontage(UClass *Class, FVector Location, FRotator rotation);
	/**
	 *装备武器
	 */
};
