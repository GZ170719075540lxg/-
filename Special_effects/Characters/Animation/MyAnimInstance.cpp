// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAnimInstance.h"
#include "../../Characters/Player/Operation_character.h"
#include "GameFramework/PawnMovementComponent.h" // 新增：移动组件

UMyAnimInstance::UMyAnimInstance()
{
    Speed = 0.f;
}
void UMyAnimInstance::UpdateSpeed()
{
}
void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    APawn *Pawn = TryGetPawnOwner();
    if (Pawn)
    {
        Speed = Pawn->GetVelocity().Size();
        Velocity = Pawn->GetVelocity();
        isfalling = Pawn->GetMovementComponent()->IsFalling();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("没有找到Pawn"));
    }
}

void UMyAnimInstance::NativeBeginPlay()
{
    UE_LOG(LogTemp, Warning, TEXT("-蒙太奇通知-------------------------------------------------"));
}
