// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAnimInstance.h"

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
}
void UEnemyAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    // Get the reference to the enemy
    TObjectPtr<Acharacter_my> character = Cast<Acharacter_my>(TryGetPawnOwner());
    if (character)
    {
        this->Character = character;
        UE_LOG(LogTemp, Warning, TEXT("character found"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No character found"));
    }
}