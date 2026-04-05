// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "MyItemObjector.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MyItemObjectorThrow.generated.h"

/*
可抛掷物父类


*/
UCLASS()
class RPG_TU2_API AMyItemObjectorThrow : public AMyItemObjector
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyItemObjectorThrow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
