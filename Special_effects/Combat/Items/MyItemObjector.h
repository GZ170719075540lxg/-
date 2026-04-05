// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "MyItemObjector.generated.h"

UCLASS()
class RPG_TU2_API AMyItemObjector : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyItemObjector();
	// 物品信息
	UPROPERTY(EditAnywhere, Category = "Item", meta = (DisplayName = "物品信息"))
	TObjectPtr<UItem> Item;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// 初始化物品信息
	UFUNCTION(BlueprintCallable, Category = "Item", meta = (DisplayName = "初始化物品信息"))
	void InitItem(int32 id);
};
