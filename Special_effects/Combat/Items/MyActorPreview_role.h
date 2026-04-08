// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/Base/WeaponType.h"
#include "../../Core/Base/Sock.h"
#include "../../Combat/Weapons/MyWeapons.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyActorPreview_role.generated.h"

/*
预览角色
*/

UCLASS()
class RPG_TU2_API AMyActorPreview_role : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyActorPreview_role();

	/*
	武器
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon",
			  meta = (DisplayName = "武器位置"))
	TMap<ESock, TObjectPtr<UStaticMeshComponent>> WeaponComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "攻击",
			  meta = (DisplayName = "武器"))
	TObjectPtr<UMyWeapons> Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "组件",
			  meta = (DisplayName = "弹簧臂"))
	TObjectPtr<USpringArmComponent> SpringArm;
	/*

	骨骼网格体
*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	/*
	武器
	*/
	/*
	武器
	*/
	// 装备武器
	UFUNCTION(BlueprintCallable, Category = "Character|武器",
			  meta = (DisplayName = "装备武器"))
	void EquipWeapon(UStaticMesh *WeaponMesh, FName SocketName, UStaticMeshComponent *temp);
	/*
	// 获取离自己最近对象位置
	// */
	// UFUNCTION(BlueprintCallable, Category = "Character|攻击",
	// 		  meta = (DisplayName = "获取离自己最近对象位置"))
	// FVector GetNearestTargetLocation();

	/*
	装备不同类型武器
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|武器",
			  meta = (DisplayName = "装备不同类型武器"))
	void EquipWeaponBytype();
	/*
	清理武器
	*/
	UFUNCTION(BlueprintCallable, Category = "Character|武器",
			  meta = (DisplayName = "清理武器"))
	void ClearWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
