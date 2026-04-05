// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAnimNotifyState.h"
#include "../../Characters/Player/character_my.h"

void UMyAnimNotifyState::NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, float TotalDuration)
{

    UE_LOG(LogTemp, Warning, TEXT("Notify Begin"));
    Acharacter_my *character = Cast<Acharacter_my>(MeshComp->GetOwner());
    if (character)
    {
        character->AttackTrackTimer();
    }
}
void UMyAnimNotifyState::NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation)
{
    UE_LOG(LogTemp, Warning, TEXT("Notify End"));
    Acharacter_my *character = Cast<Acharacter_my>(MeshComp->GetOwner());
    if (character)
    {
        character->AttackTrackStop();
    }
}