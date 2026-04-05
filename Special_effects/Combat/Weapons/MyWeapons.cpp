// Fill out your copyright notice in the Description page of Project Settings.
#include "MyWeapons.h"
#include "../../Characters/Player/Operation_character.h"
#include "../Items/MyItemObjectorThrow.h"

// Sets default values for this component's properties
UMyWeapons::UMyWeapons()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UMyWeapons::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UMyWeapons::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}
void UMyWeapons::DrawBowMontage(UClass *Class, FVector Location, FRotator rotation)
{
	// 位置
	GetWorld()->SpawnActor<AMyItemObjectorThrow>(Class, Location, rotation);
}