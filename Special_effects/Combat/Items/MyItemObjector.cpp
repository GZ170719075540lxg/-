// Fill out your copyright notice in the Description page of Project Settings.

#include "MyItemObjector.h"
#include "../../Core/Managers/MyGameInstance.h"

// Sets default values
AMyItemObjector::AMyItemObjector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Item = CreateDefaultSubobject<UItem>(TEXT("Item"));
}

// Called when the game starts or when spawned
void AMyItemObjector::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyItemObjector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyItemObjector::InitItem(int32 id)
{
	this->Item->id = id;
	this->Item->backtype = Cast<UMyGameInstance>(GetGameInstance())->ItemTable->FindRow<FMyDataTable>(FName(FString::FromInt(id)), "")->ItemType;
}
