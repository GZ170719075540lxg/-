// Fill out your copyright notice in the Description page of Project Settings.

#include "MyItemObjectorThrow.h"
#include "../../Core/Managers/MyGameInstance.h"

// Sets default values
AMyItemObjectorThrow::AMyItemObjectorThrow()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 初始速度
}

// Called when the game starts or when spawned
void AMyItemObjectorThrow::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyItemObjectorThrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}